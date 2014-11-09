#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include "object.hpp"
#include "math.hpp"
#include "random.hpp"
#include "vector4.hpp"

#include "spaghetti.hpp"

using namespace Glome;

// Number of line segments used to draw each curve
static const size_t SEGMENTS = 20;

// From http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
template <class T>
static T
CalculateBezierPoint(float t, T *p)
{
    float u = 1.0 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    T v = p[0] * uuu; //first term
    v += p[1] * (3 * uu * t); //second term
    v += p[2] * (3 * u * tt); //third term
    v += p[3] * ttt; //fourth term

    return v;
}

Spaghetti::Spaghetti()
{
	// TODO: add color to the spaghetti
	/*struct Vertex {
		Vector4 pos;
		Vector4 color;
	};*/

	// Random spaghetti propertiers:
	const float radius = std::max(0.003f, Random::normalDistribution(0.009f, 0.0035f));
	const float density = std::max(1500.0f, Random::normalDistribution(6000.0f, 2000.0f));

	// Number of cubic Bézier curves
	const size_t spaghetti_count = roundf(radius * density);
	std::cout << "Radius: " << radius << "\nDesity: "<< density << "\nBezier count: " << spaghetti_count << std::endl;

	// Displacement along radius
	const Matrix4 R_DISP = xw_matrix(radius);

	// Number of line segments to draw
	count = spaghetti_count * SEGMENTS;

	std::vector<Vector4> bezier((spaghetti_count * 3)+2);

	for(int i = 0; i < spaghetti_count; ++i) {
		Vector4 &p0 = bezier[i*3];
		Vector4 &m  = bezier[i*3 + 1];
		Vector4 &p1 = bezier[i*3 + 2];
		
		m =
			xy_matrix(Random::arc()) *
			xz_matrix(Random::arc()) *
			yz_matrix(Random::arc()) *
			R_DISP * Vector4::ORIGIN;
		
		Vector4 d = Random::direction();

		// Those points are outside the glome's surface, lets see how it renders.
		p0 = m + d * (radius / 2.0 * Random::zeroToOne());
		p1 = m - d * (radius / 2.0 * Random::zeroToOne());
	}
	
	bezier[spaghetti_count*3] = bezier[0];
	bezier[spaghetti_count*3 + 1] = bezier[1];
	
	Vector4 dir3d = Random::direction();
	velo = rotation(Random::normalDistribution(0.008, 0.01), dir3d[0], dir3d[1], dir3d[2]);

	// Build the Vertex Buffer Object
	{
		std::vector<Vector4> vertices;
		vertices.reserve(count);

		for(int i = 0; i < spaghetti_count; ++i) {
			Vector4 *curve = &bezier[(i*3)+1];

			for(int j = 0; j < SEGMENTS; ++j) {
				float val = j / float(SEGMENTS);
				Vector4 v = CalculateBezierPoint(val, curve);
				v.w = -1.0;
				vertices.push_back(v.normalized());
			}
		}

		assert(vertices.size() == count);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	}
}

Spaghetti::~Spaghetti()
{
	glDeleteBuffers(1, &vbo);
}

void Spaghetti::draw(Camera& c)
{
	c.pushMultMat(_t);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(c.getShader()->posAttr(), 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_LINE_LOOP, 0, count);

	c.popMat();
}

void Spaghetti::update() {
  _t = velo * _t;
}
