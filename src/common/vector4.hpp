#pragma once

#include <fstream>
#include <cstdlib>
#include <cmath>
#include "gl.hpp"

class Matrix4;

class Vector4
{
public:
	static const Vector4 CANONICAL;

	static Vector4 random_direction();

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

	Vector4 operator*=(float s) const
	{
		return (*this) * s;
	}

	float& operator[](size_t elem)
	{
		return v[elem];
	}

	void normalize()
	{
		*this = (*this) * (1.0 / length());
	}

	void calc_norm_w()
	{
		w = -sqrtf(1.0f - x*x - y*y - z*z);
	}

	float squared_length() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float length() const
	{
		return sqrt(squared_length());
	}

	void loadVertex() const
	{
		glVertex4fv(v);
	}

	const float* getVertex() const {return v;}
	/** Function for writing to a stream. */
	friend std::ostream& operator<<(std::ostream& o, const Vector4& v);

private:
	friend class Matrix4;

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

