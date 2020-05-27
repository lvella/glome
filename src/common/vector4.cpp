#include <iostream>

#include "vector4.hpp"
#include "vector3.hpp"

const Vector4 Vector4::ORIGIN = Vector4(0.0f, 0.0f, 0.0f, -1.0f);
const Vector4 Vector4::UP = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::FRONT = Vector4(0.0f, 0.0f, -1.0f, 0.0f);

Vector4::Vector4(const Vector3 &other, float wl)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = wl;
}

Vector4 Vector4::sqrt() const
{
	auto v = Vector3{y, z, w}.normalized()
		* std::sqrt((1.0f - x) * 0.5f);
	return Vector4{
		std::sqrt((1.0f + x) * 0.5f),
		v.x, v.y, v.z
	};
}


std::ostream&
operator<<(std::ostream& o, const Vector4& v)
{
	o << v.v[0];
	for(size_t j = 1; j < 4; ++j)
	{
		o << ", " << v.v[j];
	}
	o << '\n';
	return o;
}

Vector3 Vector4::stereo_proj() const
{
	return Vector3(x, y, z) * (1.0/(1.0f - w));
}
