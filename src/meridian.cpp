#include <math.h>
#include <GL/glew.h>
#include "4dmath.hpp"

#include "meridian.hpp"

static int dlist;

void
initialize_meridians()
{
  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 360 * 4 * sizeof(float), NULL, GL_STATIC_DRAW);
  float *vdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  for(int i = 0; i < 360; ++i)
    {
      double n = i * M_PI / 180.0;
      vdata[4*i] = cos(n);
      vdata[4*i+1] = sin(n);
      vdata[4*i+2] = 0.0f;
      vdata[4*i+3] = 0.0f;
    }

  glUnmapBuffer(GL_ARRAY_BUFFER);
  vdata = NULL;

  // Create the display list
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexPointer(4, GL_FLOAT, 0, NULL);
  glPushMatrix();

  glColor3f(1.0f, 1.0f, .0f);
  glDrawArrays(GL_LINES, 0, 360);

  xz_matrix(M_PI / 2.0).multToGL();
  glColor3f(.0f, 1.f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  yz_matrix(M_PI / 2.0).multToGL();
  glColor3f(1.0f, .0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  xw_matrix(-M_PI / 2.0).multToGL();
  glColor3f(1.0f, .0f, .0f);
  glDrawArrays(GL_LINES, 0, 360);

  yw_matrix(-M_PI / 2.0).multToGL();
  glColor3f(.0f, .0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 360);

  yz_matrix(M_PI / 2.0).multToGL();
  glColor3f(.0f, 1.0f, 0.0f);
  glDrawArrays(GL_LINES, 0, 360);

  glPopMatrix();
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEndList();
}

void
draw_meridians()
{
  glCallList(dlist);
}
