#define GL_GLEXT_PROTOTYPES
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <png.h>

#include "minimap.hpp"

using namespace std;

extern GLuint tex_minimap;
extern GLuint program;

MiniMap::MiniMap()
{
}

void glEnable2D()
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho(0.0f, 800, 600, 0.0f, -1.0f, 1.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glUseProgram(0);
}

void glDisable2D()
{
  glUseProgram(program);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, double(800) / double(600), 0.001, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void
MiniMap::draw()
{
  glEnable2D();

  glLoadIdentity();  

  int w = 800;
  int h = 600;

  int radius = 80;
  int t = 10;
  int b = t + (2 * radius);
  int l = 10;
  int r = l + (2 * radius);

  float cx = l + radius;
  float cy = t + radius;

  float dx = (radius * sin(M_PI / 8.));
  float ppx0 = cx - dx;
  float ppy = cy - (radius * cos(M_PI / 8.));
  float ppx1 = cx + dx;

  glColor3ub(14, 164, 3);
  glBegin(GL_LINES);
  glVertex2f(ppx0, ppy);
  glVertex2f(cx, cy);
  glVertex2f(ppx1, ppy);
  glVertex2f(cx, cy);

  glEnd();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex_minimap);

  glColor3ub(14, 164, 3);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(l, t);
  glTexCoord2f(1, 0);
  glVertex2f(r, t);
  glTexCoord2f(1, 1);
  glVertex2f(r, b);
  glTexCoord2f(0, 1);
  glVertex2f(l, b);
  glEnd();  

  glDisable(GL_TEXTURE_2D);

  glDisable2D();
}

void
MiniMap::update()
{
}

