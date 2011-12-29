#include <cstdlib>
#include <unordered_map>

#include "options.hpp"

#include "kbinput.hpp"

using namespace std;
using namespace Options;

namespace Input
{

namespace Kb
{
/* 
* Map: < SDLKEY , PAIR(controller_id, callback_function) > 
*/
std::unordered_map<int , std::pair<int, pfunction> > inputs;

void
register_input(int keycode, std::pair<int, Input::pfunction> id_fcallback)
{
	inputs.insert(std::make_pair(keycode, id_fcallback));
}

bool
key_event(SDL_Event e)
{
	std::unordered_map<int , std::pair<int, Input::pfunction> >::iterator it;
	//SDL_Keycode: http://wiki.libsdl.org/moin.cgi/SDLKeycodeLookup
	int k = e.key.keysym.sym;
  it = inputs.find(k);
	std::cout << "key=" << k << std::endl;

	std::pair<int, Input::pfunction> temp =	it->second;
	Input::pfunction func = temp.second;
	//TODO: Apply normalize
	if(e.type == SDL_KEYDOWN)
		func(temp.first, 1.0); 
	else if(e.type == SDL_KEYUP)
		func(temp.first, 0.0);
	std::cout << "key=" << k << " controller_id=" << temp.first << std::endl;

	return true;
}
/*bool
key_event(SDL_Event e)
{
	// SDL_Keycode 
  int k = e.key.keysym.sym;
  switch(k)
  {
  case SDLK_ESCAPE:
    if(e.type == SDL_KEYDOWN)
    {
      ship->quit();
      return false;
    }
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

  return true;
}
*/
void
mouse_motion(int x, int y)
{
 // ship->motion(x, y);
  x -= width / 2;
  y -= height / 2;

 // ship->rotate(float(x) / float(width / 2), -(float(y) / float(height / 2)));
}

void
mouse_button(int button, int state)
{
  float accel;

  bool a = (state == SDL_PRESSED);
  if(button == SDL_BUTTON_LEFT)
  {
		std::cout << "button-left: " << button << std::endl;
    accel = a ? -0.00002 : 0.0;
    //ship->move_forward(a);
  }
  else if(button == SDL_BUTTON_RIGHT)
  {
		std::cout << "button-right: " << button << std::endl;
    accel = a ? 0.00002 : 0.0;
    //ship->move_backward(a);
  }
  else
    return;

 // ship->move(accel);
}

}
}
