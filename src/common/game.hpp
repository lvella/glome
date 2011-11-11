#pragma once

#include "world.hpp"

namespace Game
{
	extern World* world;
	enum state {EXIT, MENU, WORLD};

	/** Process and renders a game frame. */
	bool frame();
	void initialize();

	/** Switches between game states. */
	void switch_state(state s);
}
