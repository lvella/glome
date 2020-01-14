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

	std::array<HalfCell, 8> get_cells() const;

private:
	std::array<Vector4, 3> walls;
	std::array<const Vector4*, 3> opposites;
};

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

		std::array<HalfCell, 8> get_cells() const;

	private:
		Vector4 walls[3][2];
	};

public:
	void collide(std::vector<VolSphere*>&& inter, std::vector<VolSphere*>&& intra);

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
