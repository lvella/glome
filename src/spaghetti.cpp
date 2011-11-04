#include <iostream>
#include <cstdlib>
#include <cmath>
#include "object.hpp"
#include "4dmath.hpp"
#include "vector4.hpp"

#include "spaghetti.hpp"

Spaghetti::Spaghetti()
{
  static const float R = 1.0 / 50.0; // Approximated radius of the spaghetti

  for(int i = 0; i < SPAGHETTI_COUNT; ++i) {
      Vector4 &p0 = p[i*3];
      Vector4 &m  = p[i*3 + 1];
      Vector4 &p1 = p[i*3 + 2];

      Vector4 dir3d((float)rand(), (float)rand(), (float)rand(), 0);
      dir3d.normalize();
      m = // Random rotation:
          rotation((float(rand()) / RAND_MAX) * 2 * M_PI, dir3d[0], dir3d[1], dir3d[2])
          // Random displacement along radius:
          * xw_matrix((float(rand()) / RAND_MAX) * R)
          // Canonical starting position:
          * Vector4::CANONICAL;

      Vector4 d = Vector4::random_direction();
      // Those points are outside the glome's surface, lets see how it renders.
      p0 = m + d * (R * float(rand()) / RAND_MAX);
      p1 = m - d * (R * float(rand()) / RAND_MAX);
  }

  p[SPAGHETTI_COUNT*3] = p[0];
  p[SPAGHETTI_COUNT*3 + 1] = p[1];

  Vector4 dir3d((float)rand(), (float)rand(), (float)rand(), 0);
  dir3d.normalize();
  velo = rotation(0.04f, dir3d[0], dir3d[1], dir3d[2]);
}

// From http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
static Vector4
CalculateBezierPoint(float t, Vector4 *p)
{
    float u = 1.0 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vector4 v = p[0] * uuu; //first term
    v += p[1] * (3 * uu * t); //second term
    v += p[2] * (3 * u * tt); //third term
    v += p[3] * ttt; //fourth term

    return v;
}

void Spaghetti::draw()
{
  glPushMatrix();
  t.multToGL();
  glBegin(GL_LINE_LOOP);

  for(int i = 0; i < SPAGHETTI_COUNT; ++i) {
      Vector4 *segment = &p[(i*3)+1];

      for(float j = 0.0f; j < 0.95f; j += 0.1f) {
          Vector4 v = CalculateBezierPoint(j, segment);
          v.loadVertex();
      }
  }
  /*
  Vector4 segment[] = {
      Vector4(-1,0,0,0),
      Vector4(-1,1,0,0),
      Vector4(1,1,0,0),
      Vector4(1,0,0,0)
  };
  for(float j = 0.0f; j < 0.95f; j += 0.1f) {
      Vector4 v = CalculateBezierPoint(j, segment);
      v.loadVertex();
      std::cout << v << '\n';
  }
  std::cout << "\n\n\n\n";
  */

  glEnd();
  glPopMatrix();
}

void Spaghetti::update() {
  t = velo * t;
}
