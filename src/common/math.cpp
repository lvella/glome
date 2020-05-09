#include "math.hpp"

#include "matrix4.hpp"
#include "qrot.hpp"

Matrix4
perspective(float fovy, float aspect, float zNear, float zFar)
{
	float f = 1.0f / std::tan(fovy / 2.0);
	float dif = zNear - zFar;
	return Matrix4(
		f/aspect, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, (zFar + zNear) / dif, 2.0 * zFar * zNear / dif,
		0.0f, 0.0f, -1.0f, 0.0f
	);
}

Matrix4
rotation(float angle, float x, float y, float z) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	const float omc = 1.0 - c;

	return Matrix4(
		x*x*omc+c, x*y*omc-z*s, x*z*omc+y*s, 0.0,
		y*x*omc+z*s, y*y*omc+c, y*z*omc-x*s, 0.0,
		x*z*omc-y*s, y*z*omc+x*s, z*z*omc+c, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

QRot qrotation(float angle, Vector3 axis)
{
	const float h = 0.5 * angle;
	const float c = std::cos(h);
	const float s = std::sin(h);
	const Vector4 q(c, s * axis.z, -s * axis.y, s * axis.x);
	return QRot(q, Vector4(q.x, q.y, q.z, -q.w));
}

Matrix4
xy_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

Matrix4
xz_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		c, 0, -s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1
	);
}

Matrix4
yz_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	);
}

Matrix4
xw_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		c, 0, 0, -s,
		0, 1, 0, 0,
		0, 0, 1, 0,
		s, 0, 0, c
	);
}

Matrix4
zw_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		1,0,0,0,
		0,1,0,0,
		0,0,c,-s,
		0,0,s,c
	);
}

Matrix4
yw_matrix(float angle) {
	const float c = std::cos(angle);
	const float s = std::sin(angle);
	return Matrix4(
		1,0,0,0,
		0,c,0,-s,
		0,0,1,0,
		0,s,0,c
	);
}

QRot xy_qrot(float angle)
{
	const float h = 0.5 * angle;
	const Vector4 q(std::cos(h), std::sin(h), 0, 0);
	return QRot(q, q);
}

QRot xz_qrot(float angle)
{
	const float h = 0.5 * angle;
	const Vector4 q(std::cos(h), 0, std::sin(h), 0);
	return QRot(q, q);
}

QRot yz_qrot(float angle)
{
	const float h = 0.5 * angle;
	const float c = std::cos(h);
	const float s = std::sin(h);
	return QRot(
		Vector4(c, 0, 0, s),
		Vector4(c, 0, 0, -s)
	);
}

QRot xw_qrot(float angle)
{
	const float h = 0.5 * angle;
	const Vector4 q(std::cos(h), 0, 0, std::sin(h));
	return QRot(q, q);
}

QRot zw_qrot(float angle)
{
	const float h = 0.5 * angle;
	const float c = std::cos(h);
	const float s = std::sin(h);
	return QRot(
		Vector4(c, s, 0, 0),
		Vector4(c, -s, 0, 0)
	);
}

QRot yw_qrot(float angle)
{
	const float h = 0.5 * angle;
	const float c = std::cos(h);
	const float s = std::sin(h);
	return QRot(
		Vector4(c, 0, -s, 0),
		Vector4(c, 0, s, 0)
	);
}

QRot nlerp(const QRot& a, const QRot& b, float t)
{
	return QRot(
		nlerp(a.l, b.l, t),
		nlerp(a.r, b.r, t)
	);
}

Vector4 nlerp(const Vector4& a, const Vector4& b, float t)
{
	return (a + (b - a) * t).normalized();
}
