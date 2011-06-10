#include <cstdlib>

#include "jsinput.hpp"

using namespace std;

namespace Input {
namespace Js {

static Ship* ship;
static SDL_Joystick* js = NULL;

void initialize(int js_id)
{
  if(js)
    SDL_JoystickClose(js);

  if(SDL_NumJoysticks() > js_id) {
    js = SDL_JoystickOpen(js_id);
  } else {
    js = NULL;
  }
}

void set_ship(Ship* s)
{
  ship = s;
}

void axis_event(const SDL_JoyAxisEvent &e)
{
  static float turn_x = 0.0f, turn_y = 0.0f;

  if(e.axis == 0) {
    if(e.value < 16000 && e.value > -16000) {
      ship->move_left(false);
      ship->move_right(false);
    } else if(e.axis > 16000) {
      ship->move_right(true);
    } else {
      ship->move_left(true);
    }
  } else if(e.axis == 1) {
      if(e.value < 16000 && e.value > -16000) {
        ship->move_down(false);
        ship->move_up(false);
      } else if(e.axis > 16000) {
        ship->move_up(true);
      } else {
        ship->move_down(true);
      }
  } else if(e.axis == 2 || e.axis == 3) {
      if(e.axis == 2) {
        turn_x = e.value / 32768.0f;
      } else {
        turn_y = e.value / 32768.0f;
      }
      ship->rotate(turn_x, turn_y);
   }
}

void button_event(const SDL_JoyButtonEvent &e)
{
  // TO BE CONTINUED
}

}
}
