#pragma once

#include <chrono>
#include "runcontext.hpp"

namespace Game
{
	constexpr unsigned MIN_FPS = 50;
	extern RunContext* context;
	enum class State {MENU, WORLD};

	/** Process and renders a game frame. */
	void frame(std::chrono::duration<float> frame_time);

	void initialize();
	void shutdown();

	/** Switches between game states. */
	void switch_state(State s);
}
