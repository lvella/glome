#pragma once

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "object.hpp"
#include "4dmath.hpp"

class RandomCube: public Object
{
public:
  RandomCube()
  {
    srand(time(NULL));
    t = xy_matrix(rand()/10000.0f) * xz_matrix(rand()/10000.0f) * yz_matrix(rand()/10000.0f) *  xw_matrix(rand()/10000.0f) * yw_matrix(rand()/10000.0f) * zw_matrix(rand()/10000.0f);
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
    glPopMatrix();
  }
};
