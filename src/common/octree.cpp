#include "octree.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace Octree {

size_t exhaustive_cost(size_t count)
{
	return count * (count - 1) / 2;
}

void exhaustive_collide(
	const std::vector<VolSphere*>& elems,
	std::unordered_map<CollisionPair, float>& collisions)
{
	for(size_t i = 0; i < elems.size(); ++i) {
		for(size_t j = i + 1; j < elems.size(); ++j) {
			float cos_dist;
			if(elems[i]->intersects(*elems[j], cos_dist)) {
				collisions[{elems[i], elems[j]}] = cos_dist;
			}
		}
	}
}

void CollisionPair::notify_collision(float cos_dist) const
{
	lower->collided_with(*higher, cos_dist);
	higher->collided_with(*lower, cos_dist);
}

bool HalfCell::intersects(const VolSphere& sphere) const
{
	for(const auto& w: walls) {
		if(!sphere.intersects_great_sphere(w)) {
			return false;
		}
	}
	return true;
}

std::vector<HalfCell> HalfCell::get_cells() const
{
	return split_cell(*this);
}

Hypercube::Cell::Cell(uint8_t center_axis, float center_value)
{
	// The center must be normalized:
	assert(center_value == 1.0f || center_value == -1.0f);

	// For each non-center axis, we have two walls
	// in the cell, to a total of 6.
	// The great sphere center of each pair of opposing
	// walls is obtained by rotating the cell center
	// by 45° along that axis to both directions.
	for(uint8_t i = 0; i < 4; ++i) {
		uint8_t ax_idx = i;
		if(i == center_axis) {
			continue;
		} else if (i > center_axis) {
			ax_idx = i - 1;
		}

		for(uint8_t j = 0; j < 2; ++j) {
			float dir = int8_t(j * 2) - 1;

			Vector4 c{0, 0, 0, 0};
			c[center_axis] = M_SQRT1_2 * center_value;
			c[i] = M_SQRT1_2 * center_value * dir;

			walls[ax_idx][j] = c;
		}
	}
}

bool Hypercube::Cell::intersects(const VolSphere& sphere) const
{
	for(const auto& dir: walls) {
		for(const Vector4& w: dir) {
			if(!sphere.intersects_great_sphere(w)) {
				return false;
			}
		}
	}
	return true;
}

void Hypercube::collide(std::vector<VolSphere*>&& objs)
{
	std::unordered_map<CollisionPair, float> collisions;
	collision_filter(*this, MAX_DEPTH, std::move(objs), collisions);

	for(const auto& c: collisions) {
		c.first.notify_collision(c.second);
	}
}

std::vector<HalfCell> Hypercube::Cell::get_cells() const
{
	return split_cell(*this);
}

}
