#pragma once

#include <array>
#include <boost/container_hash/hash_fwd.hpp>
#include <initializer_list>
#include <vector>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include "vector4.hpp"
#include "vol_sphere.hpp"

namespace Octree {

class CollisionPair {
public:
	CollisionPair(VolSphere *a, VolSphere *b) {
		if(a < b) {
			lower = a;
			higher = b;
		} else {
			lower = b;
			higher = a;
		}
	}

	bool operator==(const CollisionPair& other) const {
		return lower == other.lower && higher == other.higher;
	}

	size_t hash() const {
		std::hash<VolSphere *> h;

		size_t res = h(lower);
		boost::hash_combine(res, h(higher));
		return res;
	}

	void notify_collision(float cos_dist) const;

private:
	VolSphere *lower;
	VolSphere *higher;
};

}

namespace std
{
	template <>
	struct hash<Octree::CollisionPair>
	{
		size_t operator()(const Octree::CollisionPair& k) const
		{
			return k.hash();
		}
	};
}

namespace Octree {

size_t exhaustive_cost(size_t count);

void exhaustive_collide(const std::vector<VolSphere*>& elems,
	std::unordered_map<CollisionPair, float>& collisions
);

template<class OctreeNode>
void collision_filter(const OctreeNode& node, uint8_t depth,
		std::vector<VolSphere*>&& elems,
		std::unordered_map<CollisionPair, float>& collisions)
{
	// I estimate the cost of going down one level
	// to be about the cost of exhaustive collision
	// testing 33 elements, so there is no point in
	// going down if we have no more than 33 elements.
	//
	// Source:
	// https://www.wolframalpha.com/input/?i=x*%28x-1%29%2F2%3Dx*8*3+*+%282%2F3%29
	if(depth != 0 && elems.size() > 33) {
		std::vector<VolSphere*> cell_split[8];
		auto cells = node.get_cells();

		size_t lower_cost = 0;
		for(uint8_t i = 0; i < 8; ++i) {
			auto& split = cell_split[i];
			for(auto e: elems) {
				if(cells[i].intersects(*e)) {
					split.push_back(e);
				}
			}
			lower_cost += exhaustive_cost(split.size());
		}

		// If the collision test cost in lower level is not
		// higher than it would be in current level, descend.
		if(lower_cost <= exhaustive_cost(elems.size())) {
			elems.clear();

			for(uint8_t i = 0; i < 8; ++i) {
				collision_filter(cells[i], depth - 1,
					std::move(cell_split[i]),
					collisions
				);
			}
			return;
		}
	}

	// If execution reached here, it means it have decided
	// it is not worthwhile to descend the tree anymore,
	// so, performs exhaustive collision attempts.
	exhaustive_collide(elems, collisions);
}

// A HalfCell contains only half of the walls that
// makes up a cell (has 3 walls instead of 6). The
// other walls are common with the parent cell, so
// they are redundant and don't need to be tested
// again.
class HalfCell {
public:
	HalfCell(
		const Vector4& wall0, const Vector4* opposite0,
		const Vector4& wall1, const Vector4* opposite1,
		const Vector4& wall2, const Vector4* opposite2
	):
		walls {wall0, wall1, wall2},
		opposites {opposite0, opposite1, opposite2}
	{}

	bool intersects(const VolSphere& sphere) const;

	std::array<const Vector4*, 2> get_opposites(uint8_t i) const
	{
		return {&walls[i], opposites[i]};
	}

	std::vector<HalfCell> get_cells() const;

private:
	std::array<Vector4, 3> walls;
	std::array<const Vector4*, 3> opposites;
};

// Takes a cube cell and splits in eight.
template<class OctreeCell>
std::vector<HalfCell> split_cell(const OctreeCell& cell)
{
	std::array<const Vector4 *, 2> walls[3];
	Vector4 middle_walls[3][2];

	for(uint8_t i = 0; i < 3; ++i) {
		walls[i] = cell.get_opposites(i);

		middle_walls[i][0] = ((*walls[i][1] - *walls[i][0]) * 0.5).normalized();
		middle_walls[i][1] = ((*walls[i][0] - *walls[i][1]) * 0.5).normalized();
	}

	std::vector<HalfCell> ret;
	ret.reserve(8);

	for(uint8_t i = 0; i < 2; ++i) {
		for(uint8_t j = 0; j < 2; ++j) {
			for(uint8_t k = 0; k < 2; ++k) {
				ret.emplace_back(
					middle_walls[0][i], walls[0][i],
					middle_walls[1][j], walls[1][j],
					middle_walls[2][k], walls[2][k]
				);
			}
		}
	}

	return ret;
}

// Hypercube rooted octree
class Hypercube
{
private:
	// A cell is made up of 6 walls,
	// Each wall is a plane, i.e. a great sphere.
	// We store the center of each great sphere.
	// The cell is the area where all 6 great spheres intersect.
	class Cell {
	public:
		Cell(uint8_t center_axis, float center_value);
		bool intersects(const VolSphere& sphere) const;

		std::array<const Vector4*, 2> get_opposites(uint8_t i) const
		{
			return {&walls[i][0], &walls[i][1]};
		}

		std::vector<HalfCell> get_cells() const;

	private:
		Vector4 walls[3][2];
	};

public:
	void collide(std::vector<VolSphere*>&& objs);

	const Cell* get_cells() const
	{
		return cells;
	}

private:
	Cell cells[8] = {
		{0,  1}, {0, -1},
		{1,  1}, {1, -1},
		{2,  1}, {2, -1},
		{3,  1}, {3, -1}
	};

	static constexpr uint8_t MAX_DEPTH = 5;
};

}
