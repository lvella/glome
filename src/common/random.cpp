#include "random.hpp"

#include <fstream>
#include <cstdlib>
#include <random>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>

#include "math.hpp"

namespace Random
{
typedef boost::mt19937 Generator;
Generator gen((std::random_device())());

int range(int a, int b)
{
	boost::variate_generator<Generator*, boost::uniform_smallint<> > dist(&gen, boost::uniform_smallint<>(a, b));
	return dist();
}

float zeroToOne()
{
	static boost::variate_generator<Generator*, boost::uniform_01<float> > dist(&gen, boost::uniform_01<float>());
	return dist();
}

float arc()
{
	static boost::variate_generator<Generator*, boost::uniform_real<float> > dist(&gen, boost::uniform_real<float>(0.0f, 2 * math::pi));
	return dist();
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
