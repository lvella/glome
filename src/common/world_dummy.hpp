#pragma once

#include <vector>
#include "world_local.hpp"
#include "spaghetti.hpp"
#include "randomcube.hpp"
#include "supernova.hpp"

class WorldDummy : public WorldLocal
{
public:
	WorldDummy();
	~WorldDummy();

	void update();
	void draw();

private:
	std::vector<Glome::Drawable*> objects;
	RandomCube cube;
	Spaghetti spg;
	//Supernova nova;
};

