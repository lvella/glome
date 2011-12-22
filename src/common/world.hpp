#pragma once

#include "controller.hpp"
#include "render.hpp"
#include "ship.hpp"

/** Every game mode should derive this class. */
class World
{
public:
	static void initialize();

protected:
	Controller* _ctrl;
	Render* _render;
	std::vector<Ship*> players;
};

