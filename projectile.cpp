#include <cmath>
#include <deque>
#include "projectile.hpp"

static std::deque<Projectile> shots;

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
  for(unsigned int i = 0; i < shots.size(); ++i)
    shots[i].draw();
}

void Projectile::draw_in_minimap()
{
  glBegin(GL_POINTS);
  for(unsigned int i = 0; i < shots.size(); ++i) {
    const Matrix4 &t = shots[i].transformation();
    glVertex4f(-t[0][3], -t[1][3], -t[2][3], -t[3][3]);
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
  glBegin(GL_LINES);
  glVertex4f(a, 0.0, 0.0, w);
  glVertex4f(-a, 0.0, 0.0, w);
  glVertex4f(0.0, a, 0.0, w);
  glVertex4f(0.0, -a, 0.0, w);
  glVertex4f(0.0, 0.0, a, w);
  glVertex4f(0.0, 0.0, -a, w);
  glEnd();
  glPopMatrix();
}

void Projectile::update()
{
  ++ttl;
  t = t * ds;
}
