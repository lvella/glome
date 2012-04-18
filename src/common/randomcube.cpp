#include <stdlib.h>
#include <math.h>
#include "object.hpp"
#include "math.hpp"
#include "random.hpp"
#include "projectile.hpp"

#include "randomcube.hpp"

using namespace Glome;

RandomCube::RandomCube()
{
  randomize();
}

RandomCube::~RandomCube()
{
}

void
RandomCube::update()
{
}

void RandomCube::draw(Camera& c)
{
  float a = 0.02;
  float w = -sqrt(1 - a*a);

  c.pushMultMat(_t);

  glBegin(GL_QUADS);

  glColor3ub(255, 0, 0);
  glVertex4f(a, a, a, w);
  glVertex4f(-a, a, a, w);
  glVertex4f(-a, -a, a, w);
  glVertex4f(a, -a, a, w);

  glColor3ub(255, 255, 0);
  glVertex4f(a, a, a, w);
  glVertex4f(-a, a, a, w);
  glVertex4f(-a, a, -a, w);
  glVertex4f(a, a, -a, w);

  glColor3ub(50, 50, 50);
  glVertex4f(a, a, a, w);
  glVertex4f(a, -a, a, w);
  glVertex4f(a, -a, -a, w);
  glVertex4f(a, a, -a, w);

  glColor3ub(0, 255, 0);
  glVertex4f(a, a, -a, w);
  glVertex4f(-a, a, -a, w);
  glVertex4f(-a, -a, -a, w);
  glVertex4f(a, -a, -a, w);

  glColor3ub(0, 0, 255);
  glVertex4f(a, -a, a, w);
  glVertex4f(-a, -a, a, w);
  glVertex4f(-a, -a, -a, w);
  glVertex4f(a, -a, -a, w);

  glColor3ub(255, 0, 255);
  glVertex4f(-a, a, a, w);
  glVertex4f(-a, -a, a, w);
  glVertex4f(-a, -a, -a, w);
  glVertex4f(-a, a, -a, w);

  glEnd();

  c.popMat();
}

void RandomCube::randomize() {
  _t = xy_matrix(Random::arc())
      * xz_matrix(Random::arc())
      * yz_matrix(Random::arc())
      * xw_matrix(Random::arc())
      * yw_matrix(Random::arc())
      * zw_matrix(Random::arc());
}
