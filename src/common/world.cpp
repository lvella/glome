#include "gl.hpp"
#include "shader.hpp"
#include "world.hpp"

void World::initialize()
{
#include "world.vertex.glsl.hpp"
#include "world.fragment.glsl.hpp"

	program = setup_shader(world_vertex_glsl, world_vertex_glsl_len, world_fragment_glsl, world_fragment_glsl_len);
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

GLuint World::program;
