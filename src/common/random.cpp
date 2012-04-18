#include <fstream>
#include <cstdlib>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real.hpp>

#include "random.hpp"

namespace Random
{

Generator gen(time(0));

float zeroToOne()
{
	static boost::variate_generator<Generator*, boost::uniform_01<float> > dist(&gen, boost::uniform_01<float>());
	return dist();
}

float arc()
{
	static boost::variate_generator<Generator*, boost::uniform_real<float> > dist(&gen, boost::uniform_real<float>(0.0f, 2 * M_PI));
	return dist();
}

Vector4 direction()
{
	struct proxy {
		typedef float* iterator;
		proxy(int n) {
			v.w = 0.0f;
		}
		float* begin() {
			return v.getVertex();
		}
		float* end() {
			return v.getVertex() + 3;
		}

		Vector4 v;
	};

	static boost::variate_generator<Generator*, boost::uniform_on_sphere<float, proxy> > dist(&gen, boost::uniform_on_sphere<float, proxy>(3));

	return dist().v;
}

// TODO: This is wrong:
/*Vector4 Vector4::random_direction() {
    Vector4 ret;

    for(int i = 0; i < 4; ++i)
        ret.v[i] = rand() - (RAND_MAX / 2);

    ret.normalize();

    return ret;
}*/

}
