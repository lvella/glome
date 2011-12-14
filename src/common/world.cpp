#include "gl.hpp"
#include "world.hpp"

void world::initialize()
{
	const char *sources[] = { "world.vert", "world.frag", "no_texture.frag", NULL };

	shader.setup_shader(sources);

	shader_uniform_projection = glGetUniformLocation(shader.program(), "projection");
	shader_uniform_camera = glGetUniformLocation(shader.program(), "camera");
}

void
world::setup_display()
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
}

Shader World::shader;
GLint World::shader_uniform_camera;
GLint World::shader_uniform_projection;

