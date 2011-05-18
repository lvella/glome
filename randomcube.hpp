#pragma once

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "object.hpp"
#include "4dmath.hpp"

class RandomCube: public Object
{
public:
  RandomCube():
    Object(Matrix4::IDENTITY)
  {
    srand(time(NULL));
    t = xy_matrix(rand()) * xz_matrix(rand()) * yz_matrix(rand()) *  xw_matrix(rand()) * yw_matrix(rand()) * zw_matrix(rand());
  }

  void draw()
  {
    float a = 0.02;
    float w = sqrt(1 - a*a);

    glPushMatrix();
    t.multToGL();
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

    glColor3ub(255, 255, 255);
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
    glPopMatrix();
  }
};
