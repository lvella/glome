#pragma once

#include <SDL.h>

#include "ship.hpp"

namespace Input
{
namespace Js
{
  void initialize(int js_id);
  void set_ship(Ship* s);

  void axis_event(const SDL_JoyAxisEvent &e);
  void button_event(const SDL_JoyButtonEvent &e);
}
}
