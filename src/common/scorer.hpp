#pragma once

#include <iostream>
#include <cstdint>

class Scorer
{
public:
	void add_points(uint64_t pts)
	{
		score += pts;
		std::cout << "####### Gained " << pts << " points! Total: " << score << std::endl;
	}

	uint64_t const get_points() const
	{
		return score;
	}

private:
	uint64_t score = 0;
};
