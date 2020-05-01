#include "meridian.hpp"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <GL/glew.h>

#include "math.hpp"
#include "projectile.hpp"
#include "ship.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "options.hpp"
#include "textures.hpp"
#include "minimap.hpp"

static Camera camera;
static SpaceShader map_projection;

static Shader hud;

static GLuint tex_minimap;
static GLuint tex_object;

static GLint proj_has_tex;
static GLint hud_has_tex;

// TODO: Ugly! Encapsulate this!
GLuint square_vbo;

void
MiniMap::draw(int wstart, int hstart, Renderer* rend, const Matrix4& center, std::vector<Glome::Drawable*> objs)
{
	const int b = 10;
	const int l = 10;
	const int t = 160;
	const int r = 160;

	glViewport(wstart + l, hstart + b, r, t);

	// Draw 2D green background.
	hud.enable();
	glDisableVertexAttribArray(hud.colorAttr());
	glDisable(GL_DEPTH_TEST);

	glUniform1i(hud_has_tex, 1);
	glBindTexture(GL_TEXTURE_2D, tex_minimap);
	glVertexAttrib4f(hud.colorAttr(), 0.06f, 0.64f, 0.12f, 0.55f);

	glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
	glVertexAttribPointer(hud.posAttr(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw field of vision
	glVertexAttribPointer(hud.posAttr(), 2, GL_FLOAT, GL_FALSE, 0, (void*)(16*sizeof(float)));
	glDrawArrays(GL_LINE_STRIP, 0, 3);

	// Draw ship object
	glUniform1i(hud_has_tex, 0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glVertexAttrib3f(hud.colorAttr(), 1.0f, 1.0f, 1.0f);
	glVertexAttribPointer(hud.posAttr(), 2, GL_FLOAT, GL_FALSE, 0, (void*)(8*sizeof(float)));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// From now on, use the camera with transform stack to draw objects
	camera.reset(yz_matrix(math::pi_2) * center);
	camera.pushShader(&map_projection);

	// Draw shots
	glUniform1i(proj_has_tex, 0);
	Projectile::draw_in_minimap();

	// Draw meridians
	draw_meridians(camera);

	// Draw map objects
	glUniform1i(proj_has_tex, 1);
	glBindTexture(GL_TEXTURE_2D, tex_object);
	rend->fill_minimap(objs, camera);
	glBindTexture(GL_TEXTURE_2D, 0);
	camera.popShader();
}

void
MiniMap::initialize()
{
	create_circle_texture(256, 0.9, 0, 255, tex_minimap);
	create_circle_texture(16, 0.8, 0, 255, tex_object);

	{
		glGenBuffers(1, &square_vbo);

		// Coordinates of the lines representing the field of vision
		const float cx = 0.0f;
		const float cy = 0.0f;

		const float dx = 0.5f * Options::width * sinf(CamShader::FOV_Y)
			/ Options::height;

		const float ppx0 = cx - dx;
		const float ppy = cy + cosf(asinf(dx));
		const float ppx1 = cx + dx;

		// Scale of the little arrow in the middle of the minimap
		const float a = 0.07f;

		const float v[] = {
			// Position 0: square
			-1.0f, -1.0f,
			1.0f, -1.0f,
			-1.0f, 1.0f,
			1.0f, 1.0f,

			// Position 8: ship arrow
			0, a,
			-a, -a,
			0, (-a * 0.5f),
			a, -a,

			// Position 16: FOV
			ppx0, ppy,
			cy, cy,
			ppx1, ppy
		};
		glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	}

	map_projection.setup_shader({
		"minimap/minimap.vert",
		"minimap/map_stuff.vert",
		"minimap/minimap.frag",
		"common/texture.frag"
	});
	proj_has_tex = glGetUniformLocation(map_projection.program(), "has_tex");

	hud.setup_shader({
		"minimap/hud.vert",
		"minimap/map_stuff.vert",
		"minimap/minimap.frag",
		"common/texture.frag"
	});
	hud_has_tex = glGetUniformLocation(hud.program(), "has_tex");
}
