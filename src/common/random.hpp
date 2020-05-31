#pragma once

#include <random>
#include "vector4.hpp"

namespace Random
{

using Generator = std::mt19937;
extern Generator gen;

// Integer range [a, b]
int range(int a, int b);

// Generates in the interval [0, 1).
float zeroToOne();

// Generates in the interval [0, 2*pi).
float arc();

// Generates a random point in the unit sphere (3-D point).
extern Vector3 (* const direction)();

// Generates a random point in the glome (4-D point).
extern Vector4 (* const point)();

float normalDistribution(float mean, float std_dev);

}
