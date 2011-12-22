#include <iostream>
#include <cstdlib>
#include <cmath>
#include "object.hpp"
#include "math.hpp"
#include "vector4.hpp"

#include "spaghetti.hpp"

static inline float randr()
{
	return float(rand()) / RAND_MAX;
}

Spaghetti::Spaghetti()
{
	const float R = 1.0 / 50.0; // Approximated radius of the spaghetti
	const Matrix4 R_DISP = xw_matrix(R); // Displacement along radius

	for(int i = 0; i < SPAGHETTI_COUNT; ++i) {
		Vector4 &p0 = bezier[i*3];
		Vector4 &m  = bezier[i*3 + 1];
		Vector4 &p1 = bezier[i*3 + 2];
		
		m =
		xy_matrix((float(rand()) / RAND_MAX) * 2 * M_PI) *
		xz_matrix((float(rand()) / RAND_MAX) * 2 * M_PI) *
		yz_matrix((float(rand()) / RAND_MAX) * 2 * M_PI) *
		R_DISP * Vector4::CANONICAL; 
		
		Vector4 d = Vector4::random_direction();
		// Those points are outside the glome's surface, lets see how it renders.
		p0 = m + d * (R / 2.0 * randr());
		p1 = m - d * (R / 2.0 * randr());
	}
	
	bezier[SPAGHETTI_COUNT*3] = bezier[0];
	bezier[SPAGHETTI_COUNT*3 + 1] = bezier[1];
	
	Vector4 dir3d(rand() - (RAND_MAX / 2), rand() - (RAND_MAX / 2), rand() - (RAND_MAX / 2), 0);
	dir3d.normalize();
	velo = rotation(0.04f, dir3d[0], dir3d[1], dir3d[2]);

	glGenBuffers(2, bufobjs);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	{
	        float vbo[SPAGHETTI_COUNT * 4 * 10];
	        // TODO: To be continued...
	}
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

void Spaghetti::draw(const Shader& s)
{
  glPushMatrix();
  s.setTransform(_t);
  glBegin(GL_LINE_LOOP);

  for(int i = 0; i < SPAGHETTI_COUNT; ++i) {
      Vector4 *segment = &bezier[(i*3)+1];

      for(float j = 0.0f; j < 0.95f; j += 0.1f) {
          Vector4 v = CalculateBezierPoint(j, segment);
          v.loadVertex();
      }
  }

  glEnd();
  glPopMatrix();
}

void Spaghetti::update() {
  _t = velo * _t;
}
