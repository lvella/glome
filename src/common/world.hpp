#pragma once

#include "controller.hpp"
#include "render.hpp"
#include "aship.hpp"

/** Every game mode should derive this class. */
class world
{
protected:
	controller* _ctrl;
	render* _render;
	vector<ship*> players;
};

