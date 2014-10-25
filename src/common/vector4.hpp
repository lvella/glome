#pragma once

#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "gl.hpp"

class Matrix4;

class Vector4
{
public:
	static const Vector4 ORIGIN;
    static const Vector4 UP;
    static const Vector4 FRONT;

	Vector4()
	{}

	inline Vector4(float xl, float yl, float zl, float wl)
	{
		x = xl;
		y = yl;
		z = zl;
		w = wl;
	}

	float dot(const Vector4& ref) const
	{
		return x*ref.x + y*ref.y + z*ref.z + w*ref.w;
	}

	Vector4 operator+(const Vector4& ref) const
	{
		return Vector4(x + ref.x, y + ref.y, z + ref.z, w + ref.w);
	}

	Vector4 operator+=(const Vector4& ref)
  {
		(*this) = *this + ref;
		return *this;
  }

	Vector4 operator-(const Vector4& ref) const
	{
		return Vector4(x-ref.x, y-ref.y, z-ref.z, w-ref.w);
	}

	Vector4 operator*(float s) const
	{
		return Vector4(s*x, s*y, s*z, s*w);
	}

	Vector4 operator*=(float s)
	{
		return (*this) = (*this) * s;
	}

	float& operator[](size_t elem)
	{
		return v[elem];
	}

	Vector4 normalized()
	{
		return (*this) * (1.0 / length());
	}

	float calc_norm_w() const
	{
		return -sqrtf(1.0f - x*x - y*y - z*z);
	}

	Vector4 stereo_proj() const
	{
		float factor = 1.0f - w;
		return Vector4(x/factor, y/factor, z/factor, 1.0f);
	}

	float squared_length() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float length() const
	{
		return sqrt(squared_length());
	}

	const float* getVertex() const {return v;}
	float* getVertex() {return v;}

	/** Function for writing to a stream. */
	friend std::ostream& operator<<(std::ostream& o, const Vector4& v);

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float v[4];
	};
};

