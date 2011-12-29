#pragma once

#include <iostream>
#include <utility>

#include <SDL.h>

#include "input.hpp"
#include "ship_controller.hpp"
#include "ship.hpp"
#include "game.hpp"

namespace Input
{
namespace Kb
{
  void mouse_motion(int x, int y);
  void mouse_button(int button, int state);
  bool key_event(SDL_Event key);
	void register_input(int, std::pair<int, Input::pfunction> );

	inline void normalize(float a)
	{
	}

}
}
