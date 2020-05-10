#pragma once

#include <vector>
#include "audio_world.hpp"
#include "world.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"
#include "profiling.hpp"

class WorldDummy final: public World
{
public:
	WorldDummy();
	~WorldDummy();

private:
	Audio::World audio_world;

	std::vector<Spaghetti> fsms;
};
