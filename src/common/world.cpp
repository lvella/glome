#include "gl.hpp"
#include "shader.hpp"
#include "world.hpp"

void World::initialize()
{
#include "world.vertex.glsl.hpp"
#include "world.fragment.glsl.hpp"

	shader_program = setup_shader(world_vertex_glsl, world_vertex_glsl_len, world_fragment_glsl, world_fragment_glsl_len);
	shader_attr_position = glGetAttribLocation(shader_program, "position");
	shader_uniform_modelview = glGetUniformLocation(shader_program, "modelview");
}

void World::draw_primitives(GLuint vbo, GLuint ibo, size_t len, const Matrix4& t)
{
  t.loadTo(shader_uniform_modelview);
  glEnableVertexAttribArray(shader_attr_position);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glVertexAttribPointer(shader_attr_position, 4, GL_FLOAT, GL_FALSE, 0, NULL);

  glDrawElements(GL_LINES, len, GL_UNSIGNED_SHORT, NULL);

  glDisableVertexAttribArray(shader_attr_position);
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
GLint World::shader_uniform_modelview;
