#pragma once

#include <vector>

#include "world.hpp"
#include "ship_ai.hpp"

class world_local : public world
{
protected:
	std::vector<ship_ai*> bots;
};

