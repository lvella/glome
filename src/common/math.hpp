#pragma once

#include <cmath>

#include "matrix4.hpp"
#include "vector3.hpp"

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
