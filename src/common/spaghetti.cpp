#include "spaghetti.hpp"

#include <cmath>
#include <cassert>

#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "object.hpp"
#include "math.hpp"
#include "random.hpp"
#include "vector4.hpp"
#include "color.hpp"
#include "audio_effect.hpp"
#include "projectile.hpp"
#include "supernova.hpp"
#include "rot_dir.hpp"
#include "profiling.hpp"
#include "spaghetti_fragment.hpp"

using namespace Glome;

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

// Calculates the length of the Bézier curve.
//
// As per [1], the arc length is the integral from 0 to 1 of |dB(t)/dt| dt,
// where B(t) is the 3 component vector of the Bézier curve.
// B'(t) = dB(t)/dt is given by [2].
//
// The integration is done numerically by Gauss-Kronrod Quadrature,
// from Boost [3].
//
// [1]: https://www.khanacademy.org/math/ap-calculus-bc/bc-advanced-functions-new/bc-9-3/v/parametric-curve-arc-length
// [2]: https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves
// [3]: https://www.boost.org/doc/libs/1_74_0/libs/math/doc/html/math_toolkit/gauss_kronrod.html
template <class T>
float BezierLength(T *p)
{
	using quadrature = boost::math::quadrature::gauss_kronrod<float, 31>;

	return quadrature::integrate([p] (float t) {
		float u = 1.0f - t;
		const auto d =
			(p[1] - p[0])*3*u*u +
			(p[2] - p[1])*6*u*t +
			(p[3] - p[2])*3*t*t;
		return d.length();
	}, 0.0f, 1.0f, 5, 1e-4);
};

constexpr float radius_mu = 0.011f;
constexpr float radius_sigma = 0.0045f;

// The following constants were calculated with
// the script "scripts/probabilities-calculator.py"
constexpr float dmg_mu = 32.67f;
constexpr float dmg_sigma = 34.8f;

Spaghetti::Spaghetti():
	VolSphere(std::max(0.003f, Random::normalDistribution(radius_mu, radius_sigma)))
{
	{
		float fragility_mean = Random::normalDistribution(30.f, 15.f);
		float fragility_stddev = std::max(0.1f, fragility_mean * 0.1f);
		fragility = std::normal_distribution<>{fragility_mean, fragility_stddev};
	}

	// Random spaghetti propertiers:
	const float radius = get_radius();
	const float density = std::max(1500.0f, Random::normalDistribution(6000.0f, 2000.0f));
	const Vector3 mean_color = Color::rgbFromHsv(Random::arc(),
			0.15f + 0.8 * Random::zeroToOne(),
			0.2f + 0.8 * Random::zeroToOne());

	// Number of cubic Bézier curves
	const size_t spaghetti_count = roundf(radius * density);
	assert(spaghetti_count > 1);

	// Displacement along radius
	const QRot R_DISP = xw_qrot(radius);

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

		for(int i = 0; i < spaghetti_count; ++i) {
			Vector4 *curve = &bezier[(i*3)+1];
			const float curve_length = BezierLength(curve);
			const unsigned segments = std::ceil(curve_length / SEG_SIZE);

			for(int j = 0; j < segments; ++j) {
				// Position Bezier interpolation
				float val = j / float(segments);
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

		vbuf_size = count = vertices.size();

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
	c.setQRot(get_t());

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(Shader::ATTR_POSITION, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, pos));
	glVertexAttribPointer(Shader::ATTR_COLOR, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));

	if(ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_SHORT, NULL);

	} else {
		glDrawArrays(GL_LINE_STRIP, 0, count);
	}
}

Spaghetti::Damager* Spaghetti::get_damager(const std::shared_ptr<Scorer>& scorer)
{
	if(!scorer)
	{
		return nullptr;
	}

	for(Damager &d: damage_log) {
		if(d.scorer == scorer) {
			return &d;
		}
	}
	damage_log.push_back({scorer, 0});
	return &damage_log.back();
}

uint64_t Spaghetti::compute_score(unsigned damage_done)
{
	// Radius multiplier. Smaller is better:
	const double rad = std::exp((radius_mu - get_radius()) / radius_sigma);

	// Damage multiplier. The more damage it took to die, the better.
	const double dam = std::exp((total_damage - dmg_mu) / dmg_sigma);

	const double ret = (100u * damage_done / double(total_damage)) * rad * dam;

	return std::lround(ret);
}

bool Spaghetti::update(float dt, UpdatableAdder& adder)
{
	while(!impacts.empty()) {
		auto& back = impacts.back();

		unsigned damage;
		bool survived = chip(adder, back.location, damage);
		total_damage += damage;

		Damager *dmgr = get_damager(back.scorer);
		if(dmgr) {
			dmgr->damage += damage;
		}

		if(!survived) {
			// Score is only computed if it was
			// not supernova who killed the spaghetti.
			if(dmgr) {
				for(auto& d: damage_log) {
					d.scorer->add_points(compute_score(d.damage));
				}
			}
			return false;
		}

		impacts.pop_back();
	}

	QRot velo = qrotation(
		dt * angular_speed,
		rot_axis
	) * zw_qrot(dt * speed);

	mul_t(velo);

	return true;
}


void Spaghetti::collided_with(const Collidable& other, float)
{
	if(typeid(other) == typeid(const Projectile&) ||
		typeid(other) == typeid(const Supernova&))
	{
		impacts.push_back({
			(typeid(other) == typeid(const Projectile&) ?
			 	static_cast<const Projectile&>(other).get_scorer() :
				std::shared_ptr<Scorer>{}),
			rotate_unit_vec_towards(get_world_pos(),
				other.get_world_pos(), get_radius())
		});
	}
}

bool Spaghetti::chip(UpdatableAdder& adder, const Vector4& impact_point, unsigned& damage)
{
	static TimeAccumulator& chip_time = globalProfiler.newTimer("Spaghetti chip time");
	TimeGuard timer(chip_time);

	// Sanity check
	// Must have at least 2 vertices
	assert(count > 2);
	if(count <= 2) {
		return false;
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
	damage = std::max(1l, std::lround(bullet_damage(Random::gen)));
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
		unsigned num_segs = std::max(2l, std::lround(fragility(Random::gen)));

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
				++end;
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
		adder.add(std::make_shared<SpaghettiFragment>(
			get_t(), vdata, idata[start], num_segs + 1
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
		explode(adder, idata, vdata);
		return false;
	}
	return true;
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
				--new_size;
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

void Spaghetti::explode(UpdatableAdder& adder, const std::vector<uint16_t>& idata,
	const std::vector<Vertex>& vdata) const
{
	const unsigned unique_count = idata.size() - 1;
	if(!unique_count) {
		return;
	}

	// Search first separator
	unsigned start;
	for(start = 0; start < unique_count; ++start) {
		if(idata[start] == separator) {
			break;
		}
	}
	start %= unique_count;

	uint16_t frag_vcount = 0;
	uint16_t frag_start;
	for(unsigned i = 0; i < unique_count; ++i) {
		unsigned idx = (start + i) % unique_count;

		if(frag_vcount) {
			if(idata[idx] == separator) {
				// End of fragment:
				adder.add(std::make_shared<SpaghettiFragment>(
					get_t(), vdata, frag_start, frag_vcount
				));
				frag_vcount = 0;
			} else {
				// Middle of fragment:
				++frag_vcount;
			}
		} else if(idata[idx] != separator) {
			/* Start of fragment */
			frag_vcount = 1;
			frag_start = idata[idx];
		}
	}
	if(frag_vcount) {
		adder.add(std::make_shared<SpaghettiFragment>(
			get_t(), vdata, frag_start, frag_vcount
		));
	}
}

std::normal_distribution<> Spaghetti::bullet_damage(5.0, 3.0);
