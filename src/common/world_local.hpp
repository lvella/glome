#pragma once

#include <vector>

#include "world.hpp"
#include "ship_ai.hpp"

class WorldLocal : public World
{
protected:
	std::vector<ShipAI*> bots;
};

