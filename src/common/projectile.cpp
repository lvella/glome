#include <time.h>
#include <cmath>
#include <algorithm>
#include <vector>

#include "shader.hpp"
#include "4dmath.hpp"

#include "projectile.hpp"

typedef std::vector<Projectile> SList;
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
  program_bullet = setup_shader(projectile_glsl, projectile_glsl_len);
}

void Projectile::shot(Ship *s, const Matrix4& from, float speed)
{
  shots.push_back(Projectile(s, from, speed));
}

void Projectile::update_all(const Vector4& camera_pos)
{
  size_t dead_count = 0;

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    {
      if(!i->dead())
        i->update(camera_pos);
      else
        {
          // Greater than the maximum possible squared distance (which is 9.87).
          i->order_dist = 10.0f;
          ++dead_count;
        }
    }

  std::sort(shots.begin(), shots.end());
  shots.erase(shots.end() - dead_count, shots.end());
}

void Projectile::draw_all()
{
  if(shots.size() != 0) {
    glEnable(GL_TEXTURE_2D);
    glUseProgram(program_bullet);
    glBindTexture(GL_TEXTURE_2D, texture);
    for(SList::reverse_iterator i = shots.rbegin(); i != shots.rend(); ++i)
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
      i->die();
      return true;
    }

  return false;
}

bool Projectile::collide(Ship *s)
{
  const float r = 0.01f * 0.01f;
  Vector4 p = s->transformation().position();

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i) {
    if(s != i->owner && (p - i->transformation().position()).squared_length() < r) {
      i->die();
      return true;
    }
  }

  return false;
}

Projectile::Projectile(Ship *s, const Matrix4& from, float speed):
  Object(from),
  ds(zw_matrix(-speed)),
  owner(s),
  ttl(0),
  max_ttl((2 * M_PI - 0.05) / speed),
  max_ttl_2(max_ttl / 2),
  alpha(255u)
{
}

void Projectile::draw()
{
  glPushMatrix();
  t.multToGL();
  glBegin(GL_QUADS);

  glColor4ub(255, 200, 150, alpha);
  glVertex2i(1, 1);

  glColor4ub(150, 255, 150, alpha);
  glVertex2i(-1, 1);

  glColor4ub(130, 100, 250, alpha);
  glVertex2i(-1, -1);

  glColor4ub(255, 150, 150, alpha);
  glVertex2i(1, -1);

  glEnd();
  glPopMatrix();
}

void Projectile::update(const Vector4& camera_pos)
{
  ++ttl;
  alpha = ttl < (max_ttl_2) ? 255u : 255u - (ttl - max_ttl_2) * 200 / max_ttl_2;

  t = t * ds;

  order_dist = (camera_pos - t.position()).squared_length();
}
