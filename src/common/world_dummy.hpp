#pragma once

#include <vector>
#include "audio_world.hpp"
#include "world_local.hpp"
#include "randomcube.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"
#include "profiling.hpp"

class WorldDummy : public WorldLocal
{
public:
	WorldDummy();
	~WorldDummy();

	void update();
	void draw();

private:
	ProfillingAggregator profiler;

	Audio::World audio_world;

	std::vector<Glome::Drawable*> objects;
	std::vector<Updatable*> dynamic_objects;

	RandomCube cube;
	Supernova nova;
	std::vector<Spaghetti> fsms;
};
