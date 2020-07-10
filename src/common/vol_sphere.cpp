#include "vol_sphere.hpp"

#include <cassert>

void VolSphere::set_radius(float r)
{
	assert(r >= 0);
	assert(r <= math::pi);

	radius = r;
	cos_great_dist = std::cos(std::min(r + math::pi_2, math::pi));
}

bool VolSphere::intersects(const VolSphere& other, float &cos_dist) const
{
	// Calculates the cossine of the angle between the center of
	// the two spheres.
	cos_dist = this->position().dot(other.position());

	// The following algorithm fails if the sum of the radius of the
	// spheres is greater than or equal 180°. But in this case, both
	// spheres are always touching:
	if((radius + other.radius) >= math::pi) {
		return true;
	}

	// Calculates the cossine of the sum of the radius.
	float touching_radius = std::cos(radius + other.radius);

	// True if distance between centers is smaller or equal
	// the radius touching distance
	return cos_dist >= touching_radius;
}

bool VolSphere::intersects(const VolSphere& other) const
{
	float cos_dist;
	return intersects(other, cos_dist);
}
