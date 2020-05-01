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

	explicit QRot(const Vector4& l, const Vector4& r):
		l(l), r(r)
	{}
	explicit QRot(const Matrix4& rot_mat);

	Matrix4 toMatrix4() const;

	QRot operator*(const QRot& other) const
	{
		return QRot(
			l * other.l,
			other.r * r
		);
	}

	union {
		struct {
			Vector4 l;
			Vector4 r;
		};
		float m[2][4];
	};
};

// Guarantees that the array accessor is matches the Vectors:
static_assert(offsetof(QRot, l) == offsetof(QRot, m[0]));
static_assert(offsetof(QRot, r) == offsetof(QRot, m[1]));
