#pragma once

#include <SDL.h>

#include "ship.hpp"

class Input
{
public:
  Input(Ship* s);
  virtual ~Input();
  void handle(bool& r);

private:
  Ship* ship;
  void mouse_motion(int x, int y);
  void mouse_button(int button, int state);
  void key_event(SDL_Event key);
};

