#include "gl.hpp"
#include "shader.hpp"
#include "world.hpp"

void World::initialize()
{
#include "world.vertex.glsl.hpp"
#include "world.fragment.glsl.hpp"

	shader_program = setup_shader(world_vertex_glsl, world_vertex_glsl_len, world_fragment_glsl, world_fragment_glsl_len);

	shader_attr_position = glGetAttribLocation(shader_program, "position");
	shader_attr_color = glGetAttribLocation(shader_program, "color");

	shader_uniform_modelview = glGetUniformLocation(shader_program, "modelview");
}

World::~World()
{}

void World::setup_display()
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

GLuint World::shader_program;
GLint World::shader_attr_position;
GLint World::shader_attr_color;
GLint World::shader_uniform_modelview;
