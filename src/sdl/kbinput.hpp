#pragma once

#include <functional>

#include <SDL.h>

#include "input_callback.hpp"

namespace Input
{

namespace Kb
{

	void mouse_motion(int x, int y);
	bool button_event(SDL_Event key);
	void register_button(int, input_callback* );
	void register_motion(int, std::function<void (int, float, float)> );
}

}

