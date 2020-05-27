#include "spaghetti.hpp"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <typeinfo>

#include "object.hpp"
#include "math.hpp"
#include "random.hpp"
#include "vector4.hpp"
#include "color.hpp"
#include "audio_effect.hpp"
#include "projectile.hpp"
#include "supernova.hpp"
#include "profiling.hpp"

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
	{
		float frailty_mean = Random::normalDistribution(1.5 * SEGMENTS, 0.5 * SEGMENTS);
		frailty = std::normal_distribution<>{frailty_mean, frailty_mean * 0.1};
	}

	// Random spaghetti propertiers:
	const float radius = get_radius();
	const float density = std::max(1500.0f, Random::normalDistribution(6000.0f, 2000.0f));
	const Vector3 mean_color = Color::rgbFromHsv(Random::arc(),
			0.15f + 0.8 * Random::zeroToOne(),
			0.2f + 0.8 * Random::zeroToOne());

	// Number of cubic Bézier curves
	const size_t spaghetti_count = roundf(radius * density);
	std::cout << "spaghetti_count " << spaghetti_count << std::endl;

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

		vbuf_size = count;

		p_vbo = std::make_shared<BufferObject>();
		glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
		glBufferData(GL_ARRAY_BUFFER, vbuf_size * sizeof(vertices[0]),
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
	glEnableVertexAttribArray(s.colorAttr());

	glVertexAttribPointer(s.posAttr(), 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, pos));
	glVertexAttribPointer(s.colorAttr(), 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));

	if(ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_SHORT, NULL);

	} else {
		glDrawArrays(GL_LINE_STRIP, 0, count);
	}

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

	//mul_t(velo);

	while(!bullet_impact.empty()) {
		chip(adder, bullet_impact.back());
		bullet_impact.pop_back();
	}

	return true;
}


void Spaghetti::collided_with(const Collidable& other, float cos_dist)
{
	if(typeid(other) == typeid(const Projectile&)) {
		const Vector4 impact_point = rotate_unit_vec_towards(
			position(), other.position(), get_radius()
		);

		bullet_impact.push_back(get_t().inverse() * impact_point);
	}

	if(typeid(other) == typeid(const Supernova&)) {
		if(cos_dist >= cos(other.get_radius() - get_radius())) {
			dead = true;
		}
	}
}

class Fragment final:
	public Updatable,
	public Drawable
{
public:
	Fragment(const QRot& orig_transformation,
		float orig_angular_speed,
		float orig_speed,
		const std::shared_ptr<BufferObject>& p_vbo,
		const std::vector<Spaghetti::Vertex>& vdata,
		uint16_t start, uint16_t size
	):
		p_vbo(p_vbo)
	{
		Vector4 new_origin = center_of_mass(vdata, start, size);

		offset = rotation_between_unit_vecs(
			Vector4::ORIGIN,
			new_origin
		);

		set_t(orig_transformation * offset.inverse());

		if(start + size > vdata.size()) {
			draw_count = 2;
			starts[0] = start;
			sizes[0] = vdata.size() - start;

			starts[1] = 0;
			sizes[1] = size - sizes[0];
		} else {
			draw_count = 1;
			starts[0] = start;
			sizes[0] = size;
		}
	}

	bool update(float dt, UpdatableAdder& adder) override
	{
		ttl -= dt;
		return ttl > 0.0;
	}

	void draw(Camera& c) override
	{
		auto &s = *c.getShader();
		c.pushMultQRot(get_t() * offset);

		glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
		glEnableVertexAttribArray(s.colorAttr());

		glVertexAttribPointer(s.posAttr(), 4, GL_FLOAT, GL_FALSE,
			sizeof(Spaghetti::Vertex),
			(GLvoid*) offsetof(Spaghetti::Vertex, pos));

		glVertexAttribPointer(s.colorAttr(), 4, GL_FLOAT, GL_FALSE,
			sizeof(Spaghetti::Vertex),
			(GLvoid*) offsetof(Spaghetti::Vertex, color));

		glMultiDrawArrays(GL_LINE_STRIP, starts, sizes, draw_count);

		c.popMat();
	}

private:
	Vector4 center_of_mass(
		const std::vector<Spaghetti::Vertex>& vdata,
		uint16_t start, uint16_t size
	) const;

	QRot offset;
	std::shared_ptr<BufferObject> p_vbo;
	uint8_t draw_count;
	GLint starts[2];
	GLsizei sizes[2];

	float ttl = 3.0;
};

static TimeAccumulator& chip_time = globalProfiler.newTimer("Spaghetti chip time");

void Spaghetti::chip(UpdatableAdder& adder, const Vector4& impact_point)
{
	TimeGuard timer(chip_time);

	// Sanity check
	// Must have at least 2 vertices
	assert(count > 2);
	if(count <= 2) {
		dead = true;
		return;
	}

	// The last element og the IBO repeats the first,
	// so subtract one.
	const unsigned unique_count = count - 1;

	// Retrieve IBO:
	std::vector<uint16_t> idata(count);
	if(ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
			0, idata.size() * sizeof(idata[0]), idata.data());
	} else {
		for(uint16_t i = 0; i < unique_count; ++i) {
			idata[i] = i;
		}
		idata.back() = 0;
	}

	std::vector<std::pair<float, unsigned>> cos_dists;
	cos_dists.reserve(unique_count);

	// Retrieve VBO:
	// Last vetex is no longer used after IBO is created, so subtract one.
	std::vector<Vertex> vdata(vbuf_size - 1);
	glBindBuffer(GL_ARRAY_BUFFER, *p_vbo);
	glGetBufferSubData(GL_ARRAY_BUFFER,
		0, vdata.size() * sizeof(vdata[0]), vdata.data());

	for(unsigned i = 0; i < unique_count; ++i) {
		if(idata[i] == separator) {
			continue;
		}
		float d = vdata[idata[i]].pos.dot(impact_point);
		cos_dists.push_back({d, i});
	}

	std::sort(cos_dists.begin(), cos_dists.end(), [] (auto& a, auto& b) {
		return a.first > b.first;
	});

	// Each damage unit strips a fragment from the spaghetti.
	unsigned damage = 1; //std::max(1l, std::lround(bullet_damage(Random::gen)));
	std::cout << "Target damage: " << damage << std::endl;
	auto sorted_iter = cos_dists.begin();
	for(unsigned i = 0; i < damage; ++i)
	{
		// Skip all vertices that have already been broken-off
		while(sorted_iter != cos_dists.end()
			&& idata[sorted_iter->second] == separator)
		{
			++sorted_iter;
		}
		if(sorted_iter == cos_dists.end()) {
			// We cycled the entire list there is no vertex left.
			// We are done damaging.
			break;
		}

		const unsigned closest_vert = sorted_iter->second;

		// Number of segments in the fragment:
		unsigned num_segs = std::max(2l, std::lround(frailty(Random::gen)));
		std::cout << "Planed fragment size: " << num_segs << std::endl;

		// Find where the fragment may start
		int start = closest_vert;
		unsigned max_segs = 0;
		for(unsigned j = 0; j < num_segs; ++j) {
			int prev = start--;
			if(start == -1) {
				start = unique_count - 1;
			}

			if(idata[start] == separator)
			{
				start = prev;
				break;
			}

			++max_segs;
			if(start == closest_vert) {
				break;
			}
		}

		// Find where the fragment may end
		{
			int end = closest_vert;
			for(unsigned j = 0; j < num_segs; ++j) {
				if(end == unique_count) {
					end = 0;
				}

				if(idata[end] == separator) {
					break;
				}

				++max_segs;
				if(end == start) {
					break;
				}
			}
		}

		// There must be at least one segment in the fragment.
		// Solitary vertices may happen in this algorithm. The ones
		// not caught here are filtered later, when compacting
		// the new IBO.
		if(!max_segs) {
			idata[start] = separator;
			--i;
			continue;
		}

		if(max_segs > num_segs) {
			// Choose where to start the fragment:
			start = (start + Random::range(0, max_segs - num_segs)) % unique_count;
		} else {
			// Limit the segment size:
			num_segs = max_segs;
		}

		// Create the fragment.
		adder.add_updatable(std::make_shared<Fragment>(
			get_t(), angular_speed, speed,
			p_vbo, vdata, idata[start], num_segs + 1
		));

		// If there is only one segment, it is because both vertices are
		// limited by separator, so both must be removed.
		if(num_segs == 1) {
			idata[start] = separator;
			idata[(start + 1) % unique_count] = separator;
		} else {
			// Remove vertices internal to the fragment:
			for(unsigned j = 1; j < num_segs; ++j) {
				unsigned idx = (start + j) % unique_count;
				idata[idx] = separator;
			}
		}
	}

	// Filter blank spaces in the IBO
	unsigned remaining_segs = filter_IBO_segments(idata);
	std::cout << "Remaining segs: " << remaining_segs << std::endl;

	if(!ibo) {
		ibo = BufferObject();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idata.size() * sizeof(idata[0]),
			idata.data(), GL_STATIC_DRAW);
	} else {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
			idata.size() * sizeof(idata[0]), idata.data());
	}
	count = idata.size();

	// If too many segments were remove, kill the spaghetti.
	if(remaining_segs < (vbuf_size - 1) * 2/5) {
		// explode();
		std::cout << "KILLED!" << std::endl;
		dead = true;
	}
}

unsigned Spaghetti::filter_IBO_segments(std::vector<uint16_t>& idata)
{
	const unsigned unique_count = idata.size() - 1;

	enum class Role: uint8_t {
		SEPARATOR,
		FIRST_VERTEX,
		ORDINARY_VERTEX
	};

	Role prev;
	if(idata[(unique_count - 1)] == separator) {
		prev = Role::SEPARATOR;
	} else if(idata[(unique_count - 2)] == separator) {
		prev = Role::FIRST_VERTEX;
	} else {
		prev = Role::ORDINARY_VERTEX;
	}

	unsigned new_size = 1;
	unsigned segment_count = 0;

	unsigned dest = 0;
	for(unsigned i = 0; i < unique_count; ++i) {
		if(idata[i] == separator) {
			if(prev == Role::ORDINARY_VERTEX) {
				idata[dest] = separator;
				dest = (dest + 1) % unique_count;
				++new_size;

				prev = Role::SEPARATOR;
			} else if(prev == Role::FIRST_VERTEX) {
				dest = (unique_count + dest - 1) % unique_count;
				prev = Role::SEPARATOR;
			}
		} else {
			idata[dest] = idata[i];
			dest = (dest + 1) % unique_count;
			++new_size;

			if(prev == Role::SEPARATOR) {
				prev = Role::FIRST_VERTEX;
			} else {
				prev = Role::ORDINARY_VERTEX;
				++segment_count;
			}
		}
	}

	idata.resize(new_size);
	if(!idata.empty()) {
		idata.back() = idata[0];
	}

	return segment_count;
}

std::normal_distribution<> Spaghetti::bullet_damage(10.0, 6.0);

// Based  on http://ndp.jct.ac.il/tutorials/infitut2/node57.html
Vector4 Fragment::center_of_mass(
	const std::vector<Spaghetti::Vertex>& vdata,
	uint16_t start, uint16_t size) const
{
	assert(size > 0);

	if(size == 1) {
		return vdata[0].pos;
	}

	Vector4 M{0, 0, 0, 0};

	const Vector4* prev = &vdata[start].pos;
	for(unsigned i = 1; i < size; ++i) {
		uint16_t idx = (start + i) % vdata.size();
		const Vector4* curr = &vdata[idx].pos;

		Vector4 delta = *curr - *prev;
		M += delta;

		prev = curr;
	}

	return M.normalized();
}
