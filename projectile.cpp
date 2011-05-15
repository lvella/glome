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

Projectile::Projectile(const Matrix4& from, const Matrix4& speed):
  t(from),
  ds(speed),
  ttl(0)
{
}

void Projectile::draw()
{
  const float w = -std::sqrt(1 - (0.05*0.05));

  glPushMatrix();
  t.multToGL();
  glColor3ub(255, 255, 255);
  glBegin(GL_LINES);
  glVertex4f(0.05, 0.0, 0.0, w);
  glVertex4f(-0.05, 0.0, 0.0, w);
  glVertex4f(0.0, 0.05, 0.0, w);
  glVertex4f(0.0, -0.05, 0.0, w);
  glVertex4f(0.0, 0.0, 0.05, w);
  glVertex4f(0.0, 0.0, -0.05, w);
  glEnd();
  glPopMatrix();
}

void Projectile::update()
{
  ++ttl;
  t = t * ds;
}
