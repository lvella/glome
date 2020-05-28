#pragma once

#include "vector3.hpp"
#include "qrot.hpp"

/** Represents a rotation direction in the 3-sphere space.
 *
 * You provide an angle, and the RotDir returns a rotation in the direction
 * it represents. Give 0, and the rotor returns identity rotation QRot(1, 1).
 */
class RotDir
{
public:
	constexpr RotDir(const Vector3& l, const Vector3& r):
		l(l), r(r)
	{}

	/** Constructs the RotDir for the shortest path between 2 unit vectors.
	 *
	 * "from" must not be collinear with "to"
	 */
	RotDir(const Vector4& from, const Vector4& to)
	{
		// Similar to rotation_between_unit_vecs(), but I only need
		// the normalized vector part. l and r forms a QRot that is
		// perpendicular to 1.
		l = (to * from.conjugate()).imaginary().normalized();
		r = (from.conjugate() * to).imaginary().normalized();
	}

	QRot operator() (float angle) const
	{
		const float a = angle * 0.5f;
		const float s = std::sin(a);
		const float c = std::cos(a);
		return QRot(
			Vector4::make_quaternion(c, l*s),
			Vector4::make_quaternion(c, r*s)
		);
	}

	Vector3 l, r;
};
