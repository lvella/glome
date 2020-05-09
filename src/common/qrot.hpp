#pragma once

#include <cstddef>

#include "matrix4.hpp"
#include "vector4.hpp"

/** 4-D rotation represented as double quaternions.
 *
 * Can be used to represent every rotation inside this game.
 * References:
 *  - https://digital.csic.es/bitstream/10261/132980/1/ON-CAYLEYS.pdf
 *  - https://en.wikipedia.org/w/index.php?title=Rotations_in_4-dimensional_Euclidean_space&oldid=950831829#Relation_to_quaternions
 */
class QRot
{
public:
	QRot()
	{}

	explicit constexpr QRot(const Vector4& l, const Vector4& r, bool normalize = true):
		l(normalize ? l.normalized(): l),
		r(normalize ? r.normalized(): r)
	{}

	explicit QRot(const Matrix4& rot_mat);

	QRot operator*(const QRot& other) const
	{
		return QRot(
			(l * other.l).normalized(),
			(other.r * r).normalized()
		);
	}

	QRot &operator*=(const QRot& other)
	{
		return (*this = *this * other);
	}

	Vector4 operator*(const Vector4& other) const
	{
		return l * other * r;
	}

	QRot inverse() const
	{
		return QRot(l.conjugate(), r.conjugate(), false);
	}

	Vector4 position() const {
		// Equivalent to (l * Vector4::ORIGIN):
		const Vector4 tmp(l.w, -l.z, l.y, -l.x);

		return tmp * r;
	};

	void loadToUniform(GLint uniform) const {
		static_assert(offsetof(QRot, r) == offsetof(QRot, l) + sizeof(QRot::l));
		glUniform4fv(uniform, 2, &l.x);
	}

	static constexpr QRot IDENTITY()
	{
		return QRot{
			Vector4(1, 0, 0, 0),
			Vector4(1, 0, 0, 0),
			false
		};
	}

	Vector4 l;
	Vector4 r;
};

std::ostream& operator<<(std::ostream& o, const QRot& r);
