#include "input.hpp"

using namespace std;

const int HEIGHT = 600;
const int WIDTH = 800;

Input::Input(Ship* s)
{
	ship = s;
  w = false;
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
        key_pressed(e.key.keysym.sym);
        break;
      case SDL_KEYUP:
        key_released(e.key.keysym.sym);
        break;
	  }
	}

  if(w)
    ship->shot();
}

void
Input::mouse_motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  ship->rotate(float(x) / float(WIDTH / 2), float(y) / float(HEIGHT / 2));
}

void
Input::mouse_button(int button, int state)
{
  float speed;

  if(button == SDL_BUTTON_LEFT)
  {
    speed = (state == SDL_PRESSED) ? -0.0003 : 0.0;
  }
  else if(button == SDL_BUTTON_RIGHT)
  {
    speed = (state == SDL_PRESSED) ? 0.0003 : 0.0;
  }

  ship->move(speed);
}

void
Input::key_pressed(int key)
{
  switch(key)
  {
    case SDLK_w:
      w = true;
  }
}

void
Input::key_released(int key)
{
  switch(key)
  {
    case SDLK_w:
      w = false;
  }
}

