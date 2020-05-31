#pragma once

#include "vol_sphere.hpp"

class Collidable: virtual public VolSphere
{
public:
	virtual ~Collidable() = default;
	virtual void collided_with(const Collidable& other, float cos_dist) = 0;
};
