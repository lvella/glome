#pragma once

#include "world.hpp"

#include <memory>
#include "meridian.hpp"

class WorldDummy final: public World
{
public:
	WorldDummy();
	~WorldDummy();

private:
	std::shared_ptr<Meridians> meridians;
};
