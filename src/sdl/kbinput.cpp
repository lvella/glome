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
unordered_map<int, input_callback* > inputs;
pair< int, std::function<void (int, float, float)> > motion_func;

void
register_button(int keycode, input_callback* id_fcallback)
{
	inputs[keycode] = id_fcallback;
}

void
register_motion(int cid, std::function<void (int, float, float)> func)
{
	motion_func.first = cid;
	motion_func.second = func;
}

bool
button_event(SDL_Event e)
{
	std::unordered_map<int , input_callback* >::iterator it;
	int k;
	bool state;

	if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
	{
		k = e.key.keysym.sym;
		state = (e.type == SDL_KEYDOWN);
	}
	else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
	{
		k = e.button.button;
		state = (e.button.state == SDL_PRESSED);
	}

	it = inputs.find(k);

	if(it == inputs.end())
		return true;

	//assert(it != inputs.end());
	input_callback* temp = it->second;
	pfunction func = temp->f;

	if(state)
		func(temp->cid, Kb::normalize(temp->e));
	else
		func(temp->cid, 0.0);

	return true;
}

void
mouse_motion(int x, int y)
{
	x -= width / 2;
	y -= height / 2;

	motion_func.second(motion_func.first, float(x) / float(width / 2), -(float(y) / float(height / 2)));
}

}
}

