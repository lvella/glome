#pragma once

#include <chrono>
#include "runcontext.hpp"

namespace Game
{
	constexpr unsigned MIN_FPS = 50;
	extern RunContext* context;

	/** Process and renders a game frame. */
	void frame(std::chrono::duration<float> frame_time);

	void initialize();
	void shutdown();
}
