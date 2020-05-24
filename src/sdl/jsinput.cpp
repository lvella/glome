#include <unordered_map>

#include "input.hpp"
#include "jsinput.hpp"

using namespace std;

namespace Input
{
namespace Js
{

/*
* Map: < SDLKEY , PAIR(controller_id, callback_function) >
*/
unordered_map<int, input_callback*> inputs;
pair<int, std::function<void(int, float, float)>> motion_func;

static SDL_Joystick* js = nullptr;

void
initialize(int js_id)
{
	if(js)
		SDL_JoystickClose(js);

	if(SDL_NumJoysticks() > js_id)
	{
		js = SDL_JoystickOpen(js_id);
	}
	else
	{
		js = nullptr;
	}
}

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

void
axis_event(const SDL_JoyAxisEvent &e)
{
	static float turn_x = 0.0f, turn_y = 0.0f;
	float nx = e.value / 32768.0f;
	float ny = -e.value / 32768.0f;

	if(e.axis == 0)
	{
		if(nx > 0)
			move_right(0, nx);
		else
			move_left(0, nx);
	}
	else if(e.axis == 1)
	{
		if(ny > 0)
			move_up(0, ny);
		else
			move_down(0, ny);
	}
	else if(e.axis == 2 || e.axis == 3)
	{
		if(e.axis == 2)
		{
			turn_x = e.value / 32768.0f;
		}
		else
		{
			turn_y = -e.value / 32768.0f;
		}

		rotate(0, turn_x, turn_y);
	}
}

bool
button_event(const SDL_JoyButtonEvent &e)
{
	std::unordered_map<int, input_callback*>::iterator it;
	int k;
	bool state;

	k = e.button;
	state = (e.state == SDL_PRESSED);

	it = inputs.find(k);

	if(it == inputs.end())
		return true;

	input_callback* temp = it->second;
	pfunction func = temp->f;

	if(state)
		func(temp->cid, normalize_button(temp->e));
	else
		func(temp->cid, 0.0);

	return true;
}

}
}

