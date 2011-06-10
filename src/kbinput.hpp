#pragma once

#include <SDL.h>

#include "ship.hpp"

class KbInput
{
public:
  KbInput(Ship* s);
  virtual ~KbInput();
  bool handle();

private:
  Ship* ship;
  void mouse_motion(int x, int y);
  void mouse_button(int button, int state);
  bool key_event(SDL_Event key);
};

