#pragma once

#include "world_local.hpp"

class WorldDummy : public WorldLocal
{
public:
	WorldDummy();
	~WorldDummy();

	void update();
	void draw();
};

