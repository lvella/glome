#pragma once

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
	QRot() = default;
	explicit QRot(const Matrix4& rot_mat);

	Matrix4 toMatrix4();

private:
	Vector4 l;
	Vector4 r;
};
