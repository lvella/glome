#include "qrot.hpp"

Vector4 row(const Matrix4& m, unsigned idx)
{
	return Vector4(m[idx][0], m[idx][1], m[idx][2], m[idx][3]);
}

QRot::QRot(const Matrix4& A)
{
	// To convert from a rotation matrix to double quaternions,
	// we need to do a isoclinical decomposition.

	// This was based on:
	// https://en.wikipedia.org/w/index.php?title=Rotations_in_4-dimensional_Euclidean_space&oldid=950831829#Algebra_of_4D_rotations

	// Start with the associate matrix M:
	auto M = Matrix4(
		A[0][0]+A[1][1]+A[2][2]+A[3][3], +A[1][0]-A[0][1]-A[3][2]+A[2][3],
		+A[2][0]+A[3][1]-A[0][2]-A[1][3], +A[3][0]-A[2][1]+A[1][2]-A[0][3],

		A[1][0]-A[0][1]+A[3][2]-A[2][3], -A[0][0]-A[1][1]+A[2][2]+A[3][3],
		+A[3][0]-A[2][1]-A[1][2]+A[0][3], -A[2][0]-A[3][1]-A[0][2]-A[1][3],

		A[2][0]-A[3][1]-A[0][2]+A[1][3], -A[3][0]-A[2][1]-A[1][2]-A[0][3],
		-A[0][0]+A[1][1]-A[2][2]+A[3][3], +A[1][0]+A[0][1]-A[3][2]-A[2][3],

		A[3][0]+A[2][1]-A[1][2]-A[0][3], +A[2][0]-A[3][1]+A[0][2]-A[1][3],
		-A[1][0]-A[0][1]-A[3][2]-A[2][3], -A[0][0]+A[1][1]+A[2][2]-A[3][3]
	) * 0.25;

	// Since the 2-norm of the matrix M is 1, there is at least one element
	// whose absolute value is >= 0.25. Find it, to avoid dividing by zero:
	uint8_t i, j;
	for(i = 0; i < 4; ++i) {
		for(j = 0; j < 4; ++j) {
			if(std::fabs(M[i][j]) >= 0.2) {
				goto found;
			}
		}
	}
	assert(0);
found:

	// Solve the non-linear system to separate the quaternion from M:
	auto solve = [] (float ap, float bp, float cp, float dp) {
		const float r_ap = 1.0f / ap;
		const float a = 1.0f / std::sqrt(1.0f + (bp*bp + cp*cp + dp*dp) * r_ap * r_ap);
		return Vector4(a,
			a*bp * r_ap,
			a*cp * r_ap,
			a*dp * r_ap
		);
	};

	// Get the left isoclinic rotation
	// Cycle the elements so that the first is far from zero
	Vector4 tmp = solve(
		M[ i     ][j],
		M[(i+1)%4][j],
		M[(i+2)%4][j],
		M[(i+3)%4][j]
	);

	// Cycle back, so the elements matches:
	l[ i     ] = tmp[0];
	l[(i+1)%4] = tmp[1];
	l[(i+2)%4] = tmp[2];
	l[(i+3)%4] = tmp[3];

	// Now the right isoclinic rotation:
	tmp = solve(
		M[i][ j     ],
		M[i][(j+1)%4],
		M[i][(j+2)%4],
		M[i][(j+3)%4]
	);

	// If the pivot is negative, r solutions
	// must be opposite from l solution:
	if(M[i][j] < 0) {
		tmp = -tmp;
	}

	r[ j     ] = tmp[0];
	r[(j+1)%4] = tmp[1];
	r[(j+2)%4] = tmp[2];
	r[(j+3)%4] = tmp[3];
}

std::ostream& operator<<(std::ostream& o, const QRot& v)
{
	o << "L: " << v.l << "R: " << v.r << '\n';
	return o;
}
