#include <cmath>

#include "gl.hpp"
#include "math.hpp"
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
draw_meridians(Camera &c)
{
	const Shader *s = c.getShader();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glVertexAttribPointer(s->posAttr(), 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glVertexAttrib4f(s->colorAttr(), 1.0f, 1.0f, 0.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  Matrix4 t = xz_matrix(M_PI / 2.0);
  c.pushMultMat(t);
  glVertexAttrib4f(s->colorAttr(), .0f, 1.f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);
  c.popMat();

  t *= yz_matrix(M_PI / 2.0);
  c.pushMultMat(t);
  glVertexAttrib4f(s->colorAttr(), 1.0f, .0f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);
  c.popMat();

  t *= xw_matrix(-M_PI / 2.0);
  c.pushMultMat(t);
  glVertexAttrib4f(s->colorAttr(), 1.0f, .0f, .0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);
  c.popMat();

  t *= yw_matrix(-M_PI / 2.0);
  c.pushMultMat(t);
  glVertexAttrib4f(s->colorAttr(), .0f, .0f, 1.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);
  c.popMat();

  t *= yz_matrix(M_PI / 2.0);
  c.pushMultMat(t);
  glVertexAttrib4f(s->colorAttr(), .0f, 1.0f, 0.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);
  c.popMat();
}
