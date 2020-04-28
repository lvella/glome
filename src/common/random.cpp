#include "random.hpp"

#include <fstream>
#include <cstdlib>
#include <limits>
#include <random>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/variate_generator.hpp>

#include "math.hpp"

namespace Random
{
typedef std::mt19937 Generator;
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

template<unsigned int DIM>
Vector4 point_on_spheric_surface()
{
    static_assert(DIM > 0 && DIM <= 4, "Dimension does not fit in Vector4");

    struct Proxy {
		typedef float* iterator;
		Proxy(int n) {
            for(int i = DIM; i < 4; ++i)
                v[i] = 0.0;
		}

		float* begin() {
			return v.getVertex();
		}
		float* end() {
			return v.getVertex() + DIM;
		}

		Vector4 v;
	};

    static boost::variate_generator<Generator*, boost::uniform_on_sphere<float, Proxy> > dist(&gen, boost::uniform_on_sphere<float, Proxy>(DIM));

    return dist().v;
}

float normalDistribution(float mean, float std_dev)
{
	std::normal_distribution<float> dist(mean, std_dev);

	return dist(gen);
}

Vector4 (* const direction)() = point_on_spheric_surface<3>;
Vector4 (* const point)() = point_on_spheric_surface<4>;

}
