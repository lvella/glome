#include "octree.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace Octree {

size_t exhaustive_cost(size_t inter_count, size_t intra_count)
{
	return intra_count * (2 * inter_count + intra_count - 1) / 2;
}

size_t descend_cost(size_t total_count)
{
	return
		// Fixed cost of descent (creating the cells, etc).
		// TODO: maybe should be higher?
		1

		// Number of elements times the average number of
		// sphere <-> wall tests.
		// Modeled as:
		// 8 * (3 * 1/8 + 7/8 * (1/3 * 1 + 1/3 * 2 + 1/3 * 3))
		// which equals 17.
		+ total_count * 17;
}

static void single_collide(VolSphere* a, VolSphere* b,
	std::unordered_map<CollisionPair, float>& collisions)
{
	float cos_dist;
	if(a->intersects(*b, cos_dist)) {
		collisions[{a, b}] = cos_dist;
	}
}

void exhaustive_collide(
	const std::vector<VolSphere*>& inter,
	const std::vector<VolSphere*>& intra,
	std::unordered_map<CollisionPair, float>& collisions)
{
	// Collisions between inter and intra:
	for(auto a: inter) {
		for(auto b: intra) {
			single_collide(a, b, collisions);
		}
	}

	// Collisions between intras:
	for(size_t i = 0; i < intra.size(); ++i) {
		for(size_t j = i + 1; j < intra.size(); ++j) {
			single_collide(intra[i], intra[j], collisions);
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

void Hypercube::collide(std::vector<VolSphere*>&& inter, std::vector<VolSphere*>&& intra)
{
	std::unordered_map<CollisionPair, float> collisions;
	collision_filter(*this, MAX_DEPTH, std::move(inter), std::move(intra), collisions);

	for(const auto& c: collisions) {
		c.first.notify_collision(c.second);
	}
}

std::vector<HalfCell> Hypercube::Cell::get_cells() const
{
	return split_cell(*this);
}

}
