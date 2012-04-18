#pragma once

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_smallint.hpp>

#include "vector4.hpp"

namespace Random
{
typedef boost::mt19937 Generator;
extern Generator gen;

// Integer range [A, B]
template<int A, int B>
int range()
{
	static boost::variate_generator<Generator*, boost::uniform_smallint<> > dist(&gen, boost::uniform_smallint<>(A, B));
	return dist();
}

// Generates in the interval [0, 1).
float zeroToOne();

// Generates in the interval [0, 2*pi).
float arc();

// Generates a random point in the unit sphere (3-D point).
Vector4 direction();

}
