#pragma once

#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cassert>

class Matrix4;
class Vector3;

class Vector4
{
public:
	static constexpr size_t size = 4;

	static const Vector4 ORIGIN;
	static const Vector4 UP;
	static const Vector4 FRONT;

	Vector4() = default;

	constexpr Vector4(float xl, float yl, float zl, float wl):
		x(xl), y(yl), z(zl), w(wl)
	{}

	Vector4(const Vector3 &other, float wl=0.0f);

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

	Vector4 operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	/** Quaternion multiplication.
	 *
	 * Straight from wikipedia.
	 */
	Vector4 operator*(const Vector4& q) const
	{
		return Vector4(
			x*q.x - y*q.y - z*q.z - w*q.w,
			x*q.y + y*q.x + z*q.w - w*q.z,
			x*q.z - y*q.w + z*q.x + w*q.y,
			x*q.w + y*q.z - z*q.y + w*q.x
		);
	}

	Vector4 operator*(float s) const
	{
		return Vector4(s*x, s*y, s*z, s*w);
	}

	Vector4 operator*=(float s)
	{
		return (*this) = (*this) * s;
	}

	/** Quaternion square root. */
	Vector4 sqrt() const;

	float& operator[](size_t elem)
	{
		return v[elem];
	}

	/** Quaternion conjugate. */
	Vector4 conjugate() const
	{
		return Vector4(x, -y, -z, -w);
	}

	Vector4 normalized() const
	{
		return (*this) * (1.0f / length());
	}

	float calc_norm_w() const
	{
		return -std::sqrt(1.0f - x*x - y*y - z*z);
	}

	Vector3 stereo_proj() const;

	float squared_length() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float length() const
	{
		return std::sqrt(squared_length());
	}

	const float* data() const {return v;}
	float* data() {return v;}

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

