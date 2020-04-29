#pragma once

#include <cmath>

#include "matrix4.hpp"
#include "vector3.hpp"

// Mathematical constants and variations
namespace math {
	constexpr float pi = 3.141592653589793238462643383279502884L;
	constexpr float pi_2 = pi * 0.5L;
	constexpr float sqrt2 = 1.414213562373095048801688724209698079L;
	constexpr float sqrt1_2 = 0.707106781186547524400844362104849039L;
};


// Rotations from viewpoint

// This one is almost like glRotate, but in radians, and the vector must be normalized.
Matrix4 rotation(float angle, float x, float y, float z);

// Almost like gluPerspective, but in radians...
Matrix4 perspective(float fovy, float aspect, float zNear, float zFar);

Matrix4 xy_matrix(float angle);
Matrix4 xz_matrix(float angle);
Matrix4 yz_matrix(float angle);

// "Translations" from viewpoint
Matrix4 xw_matrix(float angle);
Matrix4 zw_matrix(float angle);
Matrix4 yw_matrix(float angle);

Vector4 cross(const Vector4 &a, const Vector4 &b, const Vector4 &c);
Matrix4 slerp(const Matrix4& a, const Matrix4& b, float t);

// Generic vector stuff
template <class T>
T vfloor(T v)
{
	for(float& e: v)
		e = floorf(e);

	return v;
}

template <class T>
T vmax(T v, float s)
{
	for(float& e: v)
		e = std::max(e, s);

	return v;
}

template <class T>
T vfract(T v)
{
	for(float& e: v)
		e -= floorf(e);

	return v;
}

template <class T>
T vabs(T v)
{
	for(float& e: v)
		e -= fabsf(e);

	return v;
}

template <class T>
T clamp(T min, T val, T max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}
