#include <cstdlib>

#include "input.hpp"

using namespace std;

const int HEIGHT = 600;
const int WIDTH = 800;

Input::Input(Ship* s)
{
	ship = s;
}

Input::~Input()
{
}

void
Input::handle(bool& r)
{
  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    switch(e.type)
    {
      case SDL_QUIT:
        r = false;
        break;
      case SDL_MOUSEMOTION:
        mouse_motion(e.motion.x, e.motion.y);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        mouse_button(e.button.button, e.button.state);
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        key_event(e);
        break;
	  }
	}
}

void
Input::key_event(SDL_Event e)
{
  int k = e.key.keysym.sym;
  switch(k)
  {
  case SDLK_ESCAPE:
    if(e.type == SDL_KEYDOWN)
      exit(0);
  case SDLK_SPACE:
    if(e.type == SDL_KEYDOWN)
      ship->shot(true);
    else if(e.type == SDL_KEYUP)
      ship->shot(false);
    break;
  case SDLK_w:
    if(e.type == SDL_KEYDOWN)
      ship->move_up(true);
    else if(e.type == SDL_KEYUP)
      ship->move_up(false);
    break;
  case SDLK_s:
    if(e.type == SDL_KEYDOWN)
      ship->move_down(true);
    else if(e.type == SDL_KEYUP)
      ship->move_down(false);
      break;
  case SDLK_a:
    if(e.type == SDL_KEYDOWN)
      ship->move_left(true);
    else if(e.type == SDL_KEYUP)
      ship->move_left(false);
    break;
  case SDLK_d:
    if(e.type == SDL_KEYDOWN)
      ship->move_right(true);
    else if(e.type == SDL_KEYUP)
      ship->move_right(false);
    break;
  case SDLK_q:
    if(e.type == SDL_KEYDOWN)
      ship->move_spinl(true);
    else if(e.type == SDL_KEYUP)
      ship->move_spinl(false);
    break;
  case SDLK_e:
    if(e.type == SDL_KEYDOWN)
      ship->move_spinr(true);
    else if(e.type == SDL_KEYUP)
      ship->move_spinr(false);
    break;
  }
}

void
Input::mouse_motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  ship->rotate(float(x) / float(WIDTH / 2), -(float(y) / float(HEIGHT / 2)));
}

void
Input::mouse_button(int button, int state)
{
  float accel;

  if(button == SDL_BUTTON_LEFT)
  {
    accel = (state == SDL_PRESSED) ? -0.00002 : 0.0;
  }
  else if(button == SDL_BUTTON_RIGHT)
  {
    accel = (state == SDL_PRESSED) ? 0.00002 : 0.0;
  }
  else
    return;

  ship->move(accel);
}

