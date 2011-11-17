#include <math.h>
#include <GL/glew.h>
#include "4dmath.hpp"
#include "world.hpp"

#include "meridian.hpp"

static GLuint vbo;

void
initialize_meridians()
{
  glGenBuffers(1, &vbo);
  float vdata[360*4];

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  for(int i = 0; i < 360; ++i)
    {
      double n = i * M_PI / 180.0;
      vdata[4*i] = cos(n);
      vdata[4*i+1] = sin(n);
      vdata[4*i+2] = 0.0f;
      vdata[4*i+3] = 0.0f;
    }

  glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), (GLvoid*)vdata, GL_STATIC_DRAW);
}

void
draw_meridians(const Matrix4& cam)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(World::shader_attr_position);
  glVertexAttribPointer(World::shader_attr_position, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	cam.loadTo(World::shader_uniform_modelview);
	glVertexAttrib4f(World::shader_attr_color, 1.0f, 1.0f, 0.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  Matrix4 t = cam * xz_matrix(M_PI / 2.0);
  t.loadTo(World::shader_uniform_modelview);
  glVertexAttrib4f(World::shader_attr_color, .0f, 1.f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  t *= yz_matrix(M_PI / 2.0);
  t.loadTo(World::shader_uniform_modelview);
  glVertexAttrib4f(World::shader_attr_color, 1.0f, .0f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  t *= xw_matrix(-M_PI / 2.0);
  t.loadTo(World::shader_uniform_modelview);
  glVertexAttrib4f(World::shader_attr_color, 1.0f, .0f, .0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  t *= yw_matrix(-M_PI / 2.0);
  t.loadTo(World::shader_uniform_modelview);
  glVertexAttrib4f(World::shader_attr_color, .0f, .0f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  t *= yz_matrix(M_PI / 2.0);
  t.loadTo(World::shader_uniform_modelview);
  glVertexAttrib4f(World::shader_attr_color, .0f, 1.0f, 0.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  glDisableVertexAttribArray(World::shader_attr_position);
}
