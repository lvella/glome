#include <cmath>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>

#include "matrix4.hpp"
#include "projectile.hpp"
#include "randomcube.hpp"
#include "ship.hpp"
#include "shader.hpp"

#include "minimap.hpp"

using namespace std;

extern GLuint tex_ship;
extern std::deque<Matrix4> cam_hist;
extern Ship ship;
extern RandomCube cube;

static GLuint tex_minimap, tex_object, program_map;

void
MiniMap::draw()
{
  extern const int HEIGHT;
  extern const int WIDTH;
  extern const float FOV;

/*
  // Calculate field of vision
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
*/

  // Change to minimap display area/projection.
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glViewport(10, HEIGHT - 170, 160, 160);
  glOrtho(-1, 1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);

  // Draw 2D green background.
  glUseProgram(0);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);

  glPushMatrix();
  glLoadIdentity();

  glColor3ub(14, 164, 3);
  glBindTexture(GL_TEXTURE_2D, tex_minimap);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-1, -1);
  glTexCoord2f(1, 0);
  glVertex2f(1, -1);
  glTexCoord2f(1, 1);
  glVertex2f(1, 1);
  glTexCoord2f(0, 1);
  glVertex2f(-1, 1);
  glEnd();

/*
  // Draw field of vision
  glColor3ub(14, 164, 3);
  glBegin(GL_LINES);
  glVertex2f(ppx0, ppy);
  glVertex2f(cx, cy);
  glVertex2f(ppx1, ppy);
  glVertex2f(cx, cy);
  glEnd();
*/

  // Draw objects
  glUseProgram(program_map);
  glClear(GL_DEPTH_BUFFER_BIT);
  {
    Matrix4 t = (yz_matrix(M_PI / 2) * cam_hist.back());
    t.loadToGL();

    {
      // Draw ship object
      //ship.draw();
      glPushMatrix();
      Matrix4 ts = t * ship.transformation();
      ship.transformation().multToGL();
      ts = ts.transpose();
      float a = 0.00007;
      float w = sqrt(1 - a * a);

      // Real points
      Vector4 v0(a, -a, 0, w);
      Vector4 v1(-a, -a, 0, w);
      Vector4 v2(-a, a, 0, w);
      Vector4 v3(a, a, 0, w);

      glBindTexture(GL_TEXTURE_2D, tex_object);
      glColor3ub(255, 0, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      Vector4 v(ts * v0);
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(1, 0);
      v = ts * v1;
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(1, 1);
      v = ts * v2;
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(0, 1);
      v = ts * v3;
      glVertex4f(v.x, v.y, v.z, v.w);
      glEnd();
      glPopMatrix();
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Draw shots
    Projectile::draw_all();

    {
      // Draw cube object
      //cube.draw();
      glPushMatrix();
      Matrix4 tc = t * cube.transformation();
      cube.transformation().multToGL();
      tc = tc.transpose();
      float a = 0.0000002;
      float w = sqrt(1 - a * a);

      // Real points
      Vector4 v0(a, -a, 0, w);
      Vector4 v1(-a, -a, 0, w);
      Vector4 v2(-a, a, 0, w);
      Vector4 v3(a, a, 0, w);

      glBindTexture(GL_TEXTURE_2D, tex_object);
      glColor3ub(0, 0, 255);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      Vector4 v(tc * v0);
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(1, 0);
      v = tc * v1;
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(1, 1);
      v = tc * v2;
      glVertex4f(v.x, v.y, v.z, v.w);
      glTexCoord2f(0, 1);
      v = tc * v3;
      glVertex4f(v.x, v.y, v.z, v.w);
      glEnd();
      glPopMatrix();
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, WIDTH, HEIGHT);    
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }

  // Disable 2D
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glViewport(0, 0, WIDTH, HEIGHT);    
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void
create_circle_texture(int w, float p, int a0, int a1, GLuint& tex)
{
  int i, j;
  float cx, cy, d, tex_r, tex_r_lim, x;
  unsigned char* texture = (unsigned char*)malloc(w * w * sizeof(unsigned char));
  cx = cy = (w - 1) / 2.;
  tex_r = w / 2.;
  tex_r_lim = tex_r * p;

  for(i = 0; i < w; ++i)
  {
    for(j = 0; j < w; ++j)
    {
      d = sqrt(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
      texture[(i * w) + j] = (d > tex_r) ? a0 : ((d < tex_r_lim) ? a1 : ((x *= (x = ((tex_r - d) / (tex_r - tex_r_lim)))) * a1));
    }
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, w, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  free(texture);
}

void
MiniMap::initialize()
{
  create_circle_texture(256, 0.8, 0, 142, tex_minimap);
  create_circle_texture(16, 0.8, 0, 255, tex_object);

#include "minimap_proj.glsl.hpp"
  program_map = setup_vshader(minimap_proj_glsl, minimap_proj_glsl_len);
}

