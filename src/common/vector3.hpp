#pragma once

#include "vector4.hpp"

class Vector3
{
public:
	Vector3()
	{}

	Vector3(float xl, float yl, float zl)
	{
		x = xl;
		y = yl;
		z = zl;
	}

	float& operator[](size_t elem)
	{
		return v[elem];
	}

	Vector3 operator+(const Vector3& o) const
	{
		return Vector3(x + o.x, y + o.y, z + o.z);
	}

	Vector3 operator+(float s) const
	{
		return Vector3(x + s, y + s, z + s);
	}

	Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator-(const Vector3& o) const
	{
		return Vector3(x - o.x, y - o.y, z - o.z);
	}

	Vector3 operator-(float s) const
	{
		return Vector3(x - s, y - s, z - s);
	}

	Vector3 operator*(float s) const
	{
		return Vector3(x * s, y * s, z * s);
	}

	Vector3 operator*(const Vector3& o) const
	{
		return Vector3(x * o.x, y * o.y, z * o.z);
	}

	float dot(const Vector3& o) const
	{
		return x * o.x + y * o.y + z * o.z;
	}

	Vector4 inverse_stereo_proj() const {
		float d = 1.0f + x*x + y*y + z*z;
		return Vector4(2.0f*x/d, 2.0f*y/d, 2.0f*z/d, (x*x + y*y + z*z - 1.0f) / d);
	}

	const float* getVertex() const {return v;}

	// STL-like interface
	typedef float* iterator;

	iterator begin() {
		return v;
	}

	iterator end() {
		return &v[3];
	}

private:
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};
