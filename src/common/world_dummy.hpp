#pragma once

#include <vector>
#include "audio_world.hpp"
#include "world_local.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"
#include "profiling.hpp"

class WorldDummy final: public WorldLocal
{
public:
	WorldDummy();
	~WorldDummy();

	void update(float dt) override;
	void draw() override;

private:
	ProfillingAggregator profiler;

	Audio::World audio_world;

	std::vector<Glome::Drawable*> objects;
	std::vector<Updatable*> dynamic_objects;

	Supernova nova;
	std::vector<Spaghetti> fsms;
};
