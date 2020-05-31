#pragma once

#include <cstdlib>
#include <cmath>

class Vector2
{
public:
	static constexpr size_t size = 2;

	Vector2()
	{}

	Vector2(float xl, float yl)
	{
		x = xl;
		y = yl;
	}

	float& operator[](size_t elem)
	{
		return v[elem];
	}

	Vector2 operator+(const Vector2& o) const
	{
		return Vector2(x + o.x, y + o.y);
	}

	Vector2 operator+(float s) const
	{
		return Vector2(x + s, y + s);
	}

	Vector2 operator-(const Vector2& o) const
	{
		return Vector2(x - o.x, y - o.y);
	}

	Vector2 operator*(float s) const
	{
		return Vector2(x * s, y * s);
	}

	float dot(const Vector2& o) const
	{
		return x * o.x + y * o.y;
	}

	float squared_length() const
	{
		return x*x + y*y;
	}

	float length() const
	{
		return sqrtf(squared_length());
	}

	const float* data() const {return v;}
	float* data() {return v;}

	// STL-like interface
	using iterator = float*;

	iterator begin() {
		return v;
	}

	iterator end() {
		return &v[2];
	}

	union {
		struct {
			float x;
			float y;
		};
		float v[2];
	};
};
