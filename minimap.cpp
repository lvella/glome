#include <cmath>
#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>

#include "minimap.hpp"

using namespace std;

extern GLuint tex_minimap;
extern GLuint program;

MiniMap::MiniMap()
{
}

void glEnable2D()
{
  // TODO: Investigate why fog is disabled and the lines become
  // thiner if texture is enabled on ATI Catalyst. Shader's fault?
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, 800, 600, 0.0f, -1.0f, 1.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glUseProgram(0);
}

void glDisable2D()
{
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glUseProgram(program);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void
MiniMap::draw()
{
  extern const int HEIGHT;
  extern const int WIDTH;
  extern const float FOV;

  const int radius = 80;
  const int t = 10;
  const int b = t + (2 * radius);
  const int l = 10;
  const int r = l + (2 * radius);

  const float cx = l + radius;
  const float cy = t + radius;

  const float angle = asin(WIDTH * sin(FOV * M_PI / 360.0f) / HEIGHT);
  const float dx = (radius * sin(angle));
  const float ppx0 = cx - dx;
  const float ppy = cy - (radius * cos(angle));
  const float ppx1 = cx + dx;

  glEnable2D();

  glColor3ub(14, 164, 3);
  glBegin(GL_LINES);
  glVertex2f(ppx0, ppy);
  glVertex2f(cx, cy);
  glVertex2f(ppx1, ppy);
  glVertex2f(cx, cy);

  glEnd();

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

  glBindTexture(GL_TEXTURE_2D, 0);

  glDisable2D();
}

void
MiniMap::update()
{
}

void
MiniMap::load_texture()
{
  int i, j, tex_h, tex_w;
  float cx, cy, d, tex_r, tex_r_lim, x;
  unsigned char texture[256 * 256];
  cx = cy = 127.5;
  tex_h = tex_w = 256;
  tex_r = 128;
  tex_r_lim = 120;

  for(i = 0; i < tex_h; ++i)
  {
    for(j = 0; j < tex_w; ++j)
    {
      d = sqrt(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
      texture[(i * tex_w) + j] = (d > tex_r) ? 0 : ((d < tex_r_lim) ? 142 : ((x *= (x = ((tex_r - d) / (tex_r - tex_r_lim)))) * 142));
    }
  }

  glGenTextures(1, &tex_minimap);
  glBindTexture(GL_TEXTURE_2D, tex_minimap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tex_w, tex_h, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}

