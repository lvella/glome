#include "render.hpp"

#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"

using namespace std;
using namespace Options;

void
Render::initialize()
{
	const char *sources[] = { "world.vert", "world.frag", "no_texture.frag", NULL };

	shader.setup_shader(sources);
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

Render::Render(vector<Ship*>* pp)
{
	players = pp;
	cam_hist.resize(10, Matrix4::IDENTITY);

	// Set non-changing camera perspective
	camera.setProjection(perspective(FOV, float(width) / float(height), 0.001f, 5.0f));
}

void
Render::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get player position
	Matrix4 center = players->at(0)->transformation().transpose();

	// Calculate camera postion
	const Matrix4 cam_offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));
	camera.reset(cam_offset * cam_hist.front());
	cam_hist.pop_front();
	cam_hist.push_back(center);

	camera.setShader(&shader);

	draw_meridians(shader);
	cube.draw(camera);
	//spg.draw(camera);

	for(size_t i = 0; i < players->size(); ++i)
	{
		players->at(i)->draw(camera);
	}

	Projectile::draw_all(camera);
	MiniMap::draw(0, this, center);
}
	
void
Render::fill_minimap()
{
	Projectile::draw_in_minimap();
	MiniMap::draw_dot(cube);
	for(size_t i = 1; i < players->size(); ++i)
		MiniMap::draw_dot(*(players->at(i)));
}

CamShader Render::shader;
