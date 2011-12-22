#include "render.hpp"

#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"

using namespace std;
using namespace Options;

void
Render::initialize()
{
	const char *sources[] = { "world.vert", "world.frag", "no_texture.frag", NULL };

	shader.setup_shader(sources);

	shader_uniform_projection = glGetUniformLocation(shader.program(), "projection");
	shader_uniform_camera = glGetUniformLocation(shader.program(), "camera");
}

void
Render::setup_display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(1.5f);
	glAlphaFunc(GL_NOTEQUAL, 0.0f);

    glViewport(0, 0, width, height);
}

Render::Render(vector<Ship*>* pp):
	cam_pos(Vector4(0.0f, 0.0f, 0.0f, -1.0f))
{
	players = pp;
	cam_hist.resize(10, Matrix4::IDENTITY);
}

void
Render::draw()
{
	const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));
	const Matrix4 p = perspective(FOV, float(width) / float(height), 0.001f, 5.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 center = players->at(0)->transformation().transpose();
	Matrix4 camera = offset * cam_hist.front();

	cam_pos = Vector4(-camera[3][0], -camera[3][1], -camera[3][2], -camera[3][3]);
	cam_hist.pop_front();
	cam_hist.push_back(center);

	shader.enable();
	p.loadTo(shader_uniform_projection);
	camera.loadTo(shader_uniform_camera);

	draw_meridians(shader);
	cube.draw(shader);
	spg.draw(shader);

	for(size_t i = 0; i < players->size(); ++i)
	{
		players->at(i)->draw(shader, camera, p);
	}

	//Projectile::draw_all(p, camera);
	MiniMap::draw(0, this, center);
}
	
void
Render::fill_minimap()
{
	MiniMap::draw_dot(cube);
	for(size_t i = 1; i < players->size(); ++i)
		MiniMap::draw_dot(*(players->at(i)));
}

Shader Render::shader;
GLint Render::shader_uniform_camera;
GLint Render::shader_uniform_projection;

