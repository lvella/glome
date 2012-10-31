#pragma once

#include "runcontext.hpp"

namespace Game
{
	extern RunContext* context;
	enum state {EXIT, MENU, WORLD};

	/** Process and renders a game frame. */
	void frame();
	void initialize();

	/** Switches between game states. */
	void switch_state(state s);
}
