#pragma once

#include "runcontext.hpp"

namespace Game
{
	extern RunContext* context;
	enum class State {EXIT, MENU, WORLD};

	/** Process and renders a game frame. */
	void frame();

	void initialize();
	void shutdown();

	/** Switches between game states. */
	void switch_state(State s);
}
