#include "random.hpp"

#include <fstream>
#include <cstdlib>
#include <limits>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/variate_generator.hpp>

#include "math.hpp"

namespace Random
{
Generator gen((std::random_device())());

int range(int a, int b)
{
	std::uniform_int_distribution<> dist(a, b);
	return dist(gen);
}

float zeroToOne()
{
	return std::generate_canonical<float, std::numeric_limits<float>::digits>(gen);
}

float arc()
{
	static std::uniform_real_distribution<float> dist(0.0f, 2 * math::pi);
	return dist(gen);
}

template<typename Vector>
Vector point_on_spheric_surface()
{
	struct Proxy {
		typedef float* iterator;

		Proxy(size_t n) {
			assert(n == Vector::size);
		}

		float* begin() {
			return v.data();
		}

		float* end() {
			return v.data() + Vector::size;
		}

		Vector v;
	};

	static boost::variate_generator<Generator*, boost::uniform_on_sphere<float, Proxy> > dist(&gen, boost::uniform_on_sphere<float, Proxy>(Vector::size));

	return dist().v;
}

float normalDistribution(float mean, float std_dev)
{
	std::normal_distribution<float> dist(mean, std_dev);

	return dist(gen);
}

Vector3 (* const direction)() = point_on_spheric_surface<Vector3>;
Vector4 (* const point)() = point_on_spheric_surface<Vector4>;

}
