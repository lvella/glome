#include <math.h>
#include <deque>
#include "shader.hpp"

#include "projectile.hpp"

static std::deque<Projectile> shots;

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

void Projectile::shot(const Matrix4& from, const Matrix4& speed)
{
  shots.push_back(Projectile(from, speed));
}

void Projectile::update_all()
{
  while(!shots.empty() && shots[0].dead())
    shots.pop_front();

  for(unsigned int i = 0; i < shots.size(); ++i)
    shots[i].update();
}

void Projectile::draw_all()
{
  if(shots.size() != 0) {
    glDisable(GL_TEXTURE_2D);
    glUseProgram(program_bullet);
    glBindTexture(GL_TEXTURE_2D, texture);
    for(unsigned int i = 0; i < shots.size(); ++i)
      shots[i].draw();
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }
}

void Projectile::draw_in_minimap()
{
  glBegin(GL_POINTS);
  for(unsigned int i = 0; i < shots.size(); ++i) {
    shots[i].transformation().position().loadVertex();
  }
  glEnd();
}

Projectile::Projectile(const Matrix4& from, const Matrix4& speed):
  Object(from),
  ds(speed),
  ttl(0),
  max_ttl((2 * M_PI) / 0.05)
{
}

void Projectile::draw()
{
  const float a = 0.0025;
  const float w = -std::sqrt(1 - (a*a));

  glPushMatrix();
  t.multToGL();
  glColor3ub(255, 255, 255);
  glBegin(GL_QUADS);
  glVertex2i(1, 1);
  glVertex2i(-1, 1);
  glVertex2i(-1, -1);
  glVertex2i(1, -1);
  glEnd();
  glPopMatrix();
}

void Projectile::update()
{
  ++ttl;
  t = t * ds;
}
