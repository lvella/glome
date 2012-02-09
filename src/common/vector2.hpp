#pragma once

#include <cstdlib>

class Vector2
{
public:
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

	const float* getVertex() const {return v;}

private:
	union {
		struct {
			float x;
			float y;
		};
		float v[2];
	};
};
