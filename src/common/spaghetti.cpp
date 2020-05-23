#include "spaghetti.hpp"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <memory>
#include <mutex>
#include <typeinfo>

#include "object.hpp"
#include "math.hpp"
#include "random.hpp"
#include "vector4.hpp"
#include "color.hpp"
#include "audio_effect.hpp"
#include "projectile.hpp"
#include "supernova.hpp"

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

// From http://paulbourke.net/miscellaneous/interpolation/
template <class T>
static T
CubicInterpolate(
	T y0, T y1,
	T y2, T y3,
	float mu)
{
	T a0,a1,a2,a3;
	float mu2;

	mu2 = mu*mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;

	return (a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}

Spaghetti::Spaghetti():
	VolSphere(std::max(0.003f, Random::normalDistribution(0.011f, 0.0045f)))
{
	// Random spaghetti propertiers:
	const float radius = get_radius();
	const float density = std::max(1500.0f, Random::normalDistribution(6000.0f, 2000.0f));
	const Vector3 mean_color = Color::rgbFromHsv(Random::arc(),
			0.15f + 0.8 * Random::zeroToOne(),
			0.2f + 0.8 * Random::zeroToOne());

	// Number of cubic Bézier curves
	const size_t spaghetti_count = roundf(radius * density);

	// Displacement along radius
	const QRot R_DISP = xw_qrot(radius);

	// Number of line segments to draw
	count = spaghetti_count * SEGMENTS;

	std::vector<Vector4> bezier((spaghetti_count * 3)+2);
	std::vector<Vector3> colors(spaghetti_count + 3);

	for(int i = 0; i < spaghetti_count; ++i) {
		Vector4 &p0 = bezier[i*3];
		Vector4 &m  = bezier[i*3 + 1];
		Vector4 &p1 = bezier[i*3 + 2];

		m =
			xy_qrot(Random::arc()) *
			xz_qrot(Random::arc()) *
			yz_qrot(Random::arc()) *
			R_DISP * Vector4::ORIGIN;

		Vector4 d = Random::direction();

		// Those points are outside the glome's hypersurface, lets see how it renders.
		p0 = m + d * (radius / 2.0 * Random::zeroToOne());
		p1 = m - d * (radius / 2.0 * Random::zeroToOne());

		colors[i] = mean_color + Vector3(Random::direction()) * Random::normalDistribution(0.0f, 0.2);
	}

	bezier[spaghetti_count*3] = bezier[0];
	bezier[spaghetti_count*3 + 1] = bezier[1];

	colors[spaghetti_count] = colors[0];
	colors[spaghetti_count + 1] = colors[1];
	colors[spaghetti_count + 2] = colors[2];


	// Build the Vertex Buffer Object
	{
		std::vector<Vertex> vertices;
		vertices.reserve(++count);

		for(int i = 0; i < spaghetti_count; ++i) {
			Vector4 *curve = &bezier[(i*3)+1];

			for(int j = 0; j < SEGMENTS; ++j) {
				// Position Bezier interpolation
				float val = j / float(SEGMENTS);
				Vector4 v = CalculateBezierPoint(val, curve);
				v.w = -1.0;
				v = v.normalized();

				// Color cubic interpolation
				Vector3 color = CubicInterpolate(colors[i], colors[i+1], colors[i+2], colors[i+3], val);
				for(auto &c : color.v){
					c = clamp(0.0f, c, 1.0f);
				}

				// Add to the VBO to be drawn
				vertices.push_back(Vertex{v, Vector4(color, 1.0)});
			}
		}
		vertices.push_back(vertices[0]);
		assert(vertices.size() == count);

		p_vbo = std::make_shared<BufferObject>();
		glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(vertices[0]),
			vertices.data(), GL_STATIC_DRAW);
	}

	// Define movement parameters
	rot_axis = Random::direction();
	angular_speed = Random::normalDistribution(0.0, 1.2);
	speed = Random::normalDistribution(0.048, 0.024);

	// Define starting position and orientation
	set_t(
		  xy_qrot(Random::arc())
		* xz_qrot(Random::arc())
		* yz_qrot(Random::arc())
		* xw_qrot(Random::arc())
		* yw_qrot(Random::arc())
		* zw_qrot(Random::arc())
	);

	// Configure humming sound effect
	static Audio::Effect *hum_sound = Audio::Effect::getEffect("spaghetti");
	//setGain(1.0);
	play(hum_sound, true, Random::zeroToOne());
}

void Spaghetti::draw(Camera& c)
{
	auto &s = *c.getShader();
	c.pushMultQRot(get_t());

	glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glEnableVertexAttribArray(s.colorAttr());

	glVertexAttribPointer(s.posAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, pos));
	glVertexAttribPointer(s.colorAttr(), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));

	glDrawArrays(GL_LINE_STRIP, 0, count);

	c.popMat();
}

bool Spaghetti::update(float dt, UpdatableAdder& adder)
{
	if(dead) {
		return false;
	}

	QRot velo = qrotation(
		dt * angular_speed,
		rot_axis
	) * zw_qrot(dt * speed);

	mul_t(velo);

	while(!spawn.empty()) {
		adder.add_updatable(std::move(spawn.back()));
		spawn.pop_back();
	}

	return true;
}


void Spaghetti::collided_with(const Collidable& other, float cos_dist)
{
	if(typeid(other) == typeid(const Projectile&)) {
		const Vector4 impact_point = rotate_unit_vec_towards(
			position(), other.position(), get_radius()
		);

		chip(get_t().inverse() * impact_point);
	}

	if(typeid(other) == typeid(const Supernova&)) {
		if(cos_dist >= cos(other.get_radius() - get_radius())) {
			dead = true;
		}
	}
}

void Spaghetti::chip(const Vector4& impact_point)
{
	// Sanity check
	assert(count > 2);
	if(count <= 3) {
		dead = true;
		return;
	}

	// Retrieve VBO:
	glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
	Vertex *vdata = reinterpret_cast<Vertex*>(
		glMapBufferRange(GL_ARRAY_BUFFER, 0,
			count * sizeof(Vertex), GL_MAP_READ_BIT
		)
	);

	// Retrieve IBO:
	std::vector<uint16_t> idata(count);
	if(ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
			0, idata.size() * sizeof(idata[0]), idata.data());
	} else {
		for(uint16_t i = 0; i < idata.size() - 1; ++i) {
			idata[i] = i;
		}
		idata.back() = 0;
	}

	// TODO: to be continued
	// Calculate distances.

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// TODO: to be continued
	// Slice the spaghetti
	// count = ...

	if(!ibo) {
		std::cout << "NOW USING IBO!" << std::endl;
		ibo = BufferObject();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(idata[0]),
			idata.data(), GL_STATIC_DRAW);
	} else {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count, idata.data());
	}
}
