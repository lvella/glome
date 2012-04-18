#pragma once

#include "vector4.hpp"

namespace Random
{
// Integer range [a, b]
int range(int a, int b);

// Generates in the interval [0, 1).
float zeroToOne();

// Generates in the interval [0, 2*pi).
float arc();

// Generates a random point in the unit sphere (3-D point).
Vector4 direction();

}
