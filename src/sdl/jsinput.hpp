#pragma once

#include <SDL.h>

#include "input_callback.hpp"

namespace Input
{
namespace Js
{
	void initialize(int js_id);
	void register_button(int, input_callback* );
	void axis_event(const SDL_JoyAxisEvent &e);
	bool button_event(const SDL_JoyButtonEvent &e);
}
}
