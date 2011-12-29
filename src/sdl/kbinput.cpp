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
	int k = e.key.keysym.sym;
  it = inputs.find(k);
	std::pair<int, Input::pfunction> temp =	it->second;
	Input::pfunction func = temp.second;
	//FIXME: Do work using quit() instead of SDLK_ESCAPE
	switch(k)
  {
  case SDLK_ESCAPE:
    if(e.type == SDL_KEYDOWN)
      return false;
	default:
		//TODO: Apply normalize
		if(e.type == SDL_KEYDOWN)
			func(temp.first, 1); 
		else if(e.type == SDL_KEYUP)
			func(temp.first, 0);
	}
	return true;
}

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
	std::unordered_map<int , std::pair<int, Input::pfunction> >::iterator it;
	it = inputs.find(button);
	std::pair<int, Input::pfunction> temp =	it->second;
	Input::pfunction func = temp.second;

	if(button == SDL_BUTTON_LEFT)
  {
		accel = a ? -0.00002 : 0.0;
		//ship->move_forward(a);
		func(temp.first, accel);
  }
  else if(button == SDL_BUTTON_RIGHT)
  {
		accel = a ? 0.00002 : 0.0;
		// ship->move_backward(a);
		func(temp.first, accel);
  }
	else
		return;
 // ship->move(accel);
}

}
}
