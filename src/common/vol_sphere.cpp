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
	cos_dist = this->get_world_pos().dot(other.get_world_pos());

	return test_sphere_intersection(radius, other.radius, cos_dist);
}

bool VolSphere::intersects(const VolSphere& other) const
{
	float cos_dist;
	return intersects(other, cos_dist);
}
