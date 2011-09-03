#pragma once

#include <SDL.h>

#include "ship.hpp"
#include "game.hpp"

namespace Input
{
namespace Kb
{
  void set_ship(Ship* s);
  void mouse_motion(int x, int y);
  void mouse_button(int button, int state);
  bool key_event(SDL_Event key);
}
}

