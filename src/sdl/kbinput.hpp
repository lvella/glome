#pragma once

#include <iostream>
#include <utility>
#include <functional>

#include <SDL.h>

#include "events.hpp"
#include "input_callback.hpp"

namespace Input
{

namespace Kb
{

	void mouse_motion(int x, int y);
	bool button_event(SDL_Event key);
	void register_button(int, input_callback* );
	void register_motion(int, std::function<void (int, float, float)> );

	inline float normalize(int e)
	{
		switch(e)
		{
		case SHOOT:
		case MOVE_BACKWARD:
		case MOVE_RIGHT:
		case MOVE_UP:
		case MOVE_SPINL:
			return 1.0;
		case MOVE_DOWN:
		case MOVE_LEFT:
		case MOVE_SPINR:
		case MOVE_FORWARD:
			return -1.0;
		default:
			return 0.0;
		}
	}
}

}

