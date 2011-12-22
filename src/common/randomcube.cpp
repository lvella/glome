#include <stdlib.h>
#include <math.h>
#include "object.hpp"
#include "math.hpp"
#include "projectile.hpp"

#include "randomcube.hpp"

RandomCube::RandomCube()
{
  randomize();
}

void RandomCube::draw(const Shader& s)
{
  float a = 0.02;
  float w = -sqrt(1 - a*a);

  s.setTransform(_t);
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
}

void RandomCube::randomize() {
  _t = xy_matrix(rand()/10000.0f)
      * xz_matrix(rand()/10000.0f)
      * yz_matrix(rand()/10000.0f)
      * xw_matrix(rand()/10000.0f)
      * yw_matrix(rand()/10000.0f)
      * zw_matrix(rand()/10000.0f);
}
