#pragma once

#include <cmath>

#include "matrix4.hpp"
#include "vector3.hpp"
#include "qrot.hpp"

// Mathematical constants and variations
namespace math {
	constexpr float pi = 3.141592653589793238462643383279502884L;
	constexpr float pi_2 = pi * 0.5L;
	constexpr float sqrt2 = 1.414213562373095048801688724209698079L;
	constexpr float sqrt1_2 = 0.707106781186547524400844362104849039L;
};

// Almost like gluPerspective, but in radians...
Matrix4 perspective(float fovy, float aspect, float zNear, float zFar);

// Rotations around a 3-D axis from the origin point
QRot qrotation(float angle, Vector3 axis);

// "3-D Rotations" from the origin point
QRot xy_qrot(float angle);
QRot xz_qrot(float angle);
QRot yz_qrot(float angle);

// "Translations" from the origin point
QRot xw_qrot(float angle);
QRot zw_qrot(float angle);
QRot yw_qrot(float angle);

/** Normalized linear interpolation.
 *
 * This link says NLERP is a good enough replacement for SLERP:
 * http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
 */
QRot nlerp(const QRot& a, const QRot& b, float t);
Vector4 nlerp(const Vector4& a, const Vector4& b, float t);

/** Rotates unit vector "from" towards vector "to", by an amount of "angle".
 *
 * "from" and "to" must not be collinear
 */
template<typename V>
V rotate_unit_vec_towards(const V& from, const V& to, float angle)
{
	// We need an orthogonal basis made up of "from" and an orthogonal
	// vector in the direction of "to". So we calculate de rejection of "to"
	// w.r.t "from", and normalize.
	const V& a = from;
	const V b = (to - from * to.dot(from)).normalized();

	// a and b forms an orthonormal basis, so just rotate a in direction of b
	return a * std::cos(angle) + b * std::sin(angle);
}

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
