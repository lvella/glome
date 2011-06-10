#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <list>

#include "shader.hpp"
#include "4dmath.hpp"

#include "projectile.hpp"

typedef std::list<Projectile> SList;
static SList shots;

static void
create_spherical_texture(int size, GLuint& tex)
{
  unsigned char* buffer = (unsigned char *) malloc(size * size);
  float r = (float)size / 2.0;

  for(int i = 0; i < size; ++i)
  {
    for(int j = 0; j < size; ++j)
    {
      float d = hypotf(i - r, j - r);
      buffer[(i * size) + j] = d > r ? 0u : (unsigned char)nearbyint(sqrtf(r*r - d*d) / r * 255.0);
    }
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, size, size, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)buffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  free(buffer);
}

static GLuint texture, program_bullet;

void Projectile::initialize()
{
  create_spherical_texture(64, texture);

#include "projectile.glsl.hpp"
  program_bullet = setup_vshader(projectile_glsl, projectile_glsl_len);
}

void Projectile::shot(const Matrix4& from, float speed)
{
  shots.push_back(Projectile(from, speed));
}

void Projectile::update_all()
{
  while(!shots.empty() && shots.front().dead())
    shots.pop_front();

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    i->update();
}

void Projectile::draw_all()
{
  if(shots.size() != 0) {
    glEnable(GL_TEXTURE_2D);
    glUseProgram(program_bullet);
    glBindTexture(GL_TEXTURE_2D, texture);
    for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
      i->draw();
    glDisable(GL_TEXTURE_2D);
  }
}

void Projectile::draw_in_minimap()
{
  glBegin(GL_POINTS);
  for(SList::iterator i = shots.begin(); i != shots.end(); ++i) {
    i->transformation().position().loadVertex();
  }
  glEnd();
}

bool Projectile::collide(const Vector4& position, float radius)
{
  radius *= radius;

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    if((position - i->transformation().position()).squared_length() < radius) {
      shots.erase(i);
      return true;
    }

  return false;
}

Projectile::Projectile(const Matrix4& from, float s):
  Drawable(from),
  ds(zw_matrix(-s)),
  ttl(0),
  max_ttl((2 * M_PI) / s)
{
}

void Projectile::draw()
{
  glPushMatrix();
  t.multToGL();
  glBegin(GL_QUADS);

  glColor3ub(255, 200, 150);
  glVertex2i(1, 1);

  glColor3ub(150, 255, 150);
  glVertex2i(-1, 1);

  glColor3ub(130, 100, 250);
  glVertex2i(-1, -1);

  glColor3ub(255, 150, 150);
  glVertex2i(1, -1);

  glEnd();
  glPopMatrix();
}

void Projectile::update()
{
  ++ttl;
  t = t * ds;
}
