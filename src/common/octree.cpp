#include "octree.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace {

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

void CollisionPair::notify_collision(float cos_dist) const
{
	lower->collided_with(*higher, cos_dist);
	higher->collided_with(*lower, cos_dist);
}

} // anonymous namespace



namespace std
{
	template <>
	struct hash<::CollisionPair>
	{
		size_t operator()(const ::CollisionPair& k) const
		{
			return k.hash();
		}
	};
}



namespace {

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

void single_collide(VolSphere* a, VolSphere* b,
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

template<class OctreeNode>
void collision_filter(const OctreeNode& node, uint8_t depth,
		std::vector<VolSphere*>&& inter,
		std::vector<VolSphere*>&& intra,
		std::unordered_map<CollisionPair, float>& collisions)
{
	const size_t ex_cost = exhaustive_cost(inter.size(), intra.size());

	// If the cost of going down one level is greater than
	// the cost of exhaustive collision testing, stop recursion
	// as there is no point in going down.
	if(depth != 0 && descend_cost(inter.size() + intra.size()) <= ex_cost) {
		std::vector<VolSphere*> cell_splits[8][2];
		std::vector<VolSphere*> *input_lists[2] = {&inter, &intra};

		auto cells = node.get_cells();

		size_t lower_cost = 0;
		for(uint8_t i = 0; i < 8; ++i) {
			for(uint8_t j = 0; j < 2; ++j) {
				auto& split = cell_splits[i][j];
				for(auto e: *input_lists[j]) {
					if(cells[i].intersects(*e)) {
						split.push_back(e);
					}
				}
			}

			lower_cost += exhaustive_cost(
				cell_splits[i][0].size(),
				cell_splits[i][1].size()
			);
		}

		// If the collision test cost in lower level is not
		// higher than it would be in current level, descend.
		if(lower_cost <= ex_cost) {
			inter.clear();
			intra.clear();

			for(uint8_t i = 0; i < 8; ++i) {
				collision_filter(cells[i], depth - 1,
					std::move(cell_splits[i][0]),
					std::move(cell_splits[i][1]),
					collisions
				);
			}
			return;
		}
	}

	// If execution reached here, it means it have decided
	// it is not worthwhile to descend the tree anymore,
	// so, performs exhaustive collision attempts.
	exhaustive_collide(inter, intra, collisions);
}

// Takes a cube cell and splits in eight.
template<class OctreeCell>
std::array<Octree::HalfCell, 8> split_cell(const OctreeCell& cell)
{
	std::array<const Vector4 *, 2> walls[3];
	Vector4 middle_walls[3][2];

	for(uint8_t i = 0; i < 3; ++i) {
		walls[i] = cell.get_opposites(i);

		middle_walls[i][0] = ((*walls[i][1] - *walls[i][0]) * 0.5).normalized();
		middle_walls[i][1] = ((*walls[i][0] - *walls[i][1]) * 0.5).normalized();
	}

	std::array<Octree::HalfCell, 8> ret;

	uint8_t idx = 0;
	for(uint8_t i = 0; i < 2; ++i) {
		for(uint8_t j = 0; j < 2; ++j) {
			for(uint8_t k = 0; k < 2; ++k) {
				ret[idx++] = Octree::HalfCell(
					middle_walls[0][i], walls[0][i],
					middle_walls[1][j], walls[1][j],
					middle_walls[2][k], walls[2][k]
				);
			}
		}
	}

	return ret;
}

} // anonymous namespace



namespace Octree {

bool HalfCell::intersects(const VolSphere& sphere) const
{
	for(const auto& w: walls) {
		if(!sphere.intersects_great_sphere(w)) {
			return false;
		}
	}
	return true;
}

std::array<HalfCell, 8> HalfCell::get_cells() const
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

std::array<HalfCell, 8> Hypercube::Cell::get_cells() const
{
	return split_cell(*this);
}

}
