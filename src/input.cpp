#include <SDL.h>

#include "input.hpp"
#include "kbinput.hpp"
#include "jsinput.hpp"

bool
Input::handle()
{
  bool run = true;
  SDL_Event e;

  while(SDL_PollEvent(&e))
  {
    switch(e.type)
    {
      case SDL_QUIT:
        run = false;
        break;
      case SDL_MOUSEMOTION:
        Kb::mouse_motion(e.motion.x, e.motion.y);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        Kb::mouse_button(e.button.button, e.button.state);
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        run = Kb::key_event(e);
        break;
      case SDL_JOYAXISMOTION:
        Js::axis_event(e.jaxis);
        break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        Js::button_event(e.jbutton);
        break;
    }
  }

  return run;
}
