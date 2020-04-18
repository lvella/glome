#pragma once

#include <chrono>
#include "runcontext.hpp"

namespace Game
{
	extern RunContext* context;
	enum state {EXIT, MENU, WORLD};

	/** Process and renders a game frame. */
	void frame(std::chrono::duration<float> frame_time);

	void initialize();
	void shutdown();

	/** Switches between game states. */
	void switch_state(state s);
}
