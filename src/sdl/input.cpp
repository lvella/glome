#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <string>

#include <assert.h>
#include <SDL.h>

#include "config.hpp"
#include "input.hpp"
#include "kbinput.hpp"
#include "jsinput.hpp"
#include "events.hpp"

namespace Input
{
/*
* Map function names to functions poiters in controller settings file
*/
std::unordered_map<int, pfunction> pfunctions_to_inputs;

/*
* Map poiters to ShipControllers based on controller_id
*/
std::unordered_map<int, ShipController*> ship_controllers;

ShipController* create_ship_controller(int controller_id)
{
	return ship_controllers[controller_id];
}

/*
* Read the controllers settings
* [nro of inputs]
* [input_type(kb=0 or js=1)] [id_controller] [number_of_functions]
* [key] [func_key]
*/
void read_controllers_settings()
{
	std::ifstream ifs;
	int number_of_controllers, number_of_functions, controller_id, key, input_type;
	int func_key;

	std::stringstream dir;
	dir << DATA_DIR << "/controllers/input.config";
	ifs.open(dir.str().c_str(), std::ifstream::in);
	assert(ifs.is_open());
	ifs >> number_of_controllers;
	for(int i = 0; i < number_of_controllers; ++i)
	{
		ifs >> input_type >> controller_id >> number_of_functions;

		std::pair <int, ShipController* > temp;
		temp.first = controller_id;
		temp.second = new ShipController();
		ship_controllers.insert(temp);

		switch(input_type)
		{
			// 0 for Keyboard input
			case 0:
				Kb::register_motion(controller_id, rotate);
				for(int j = 0; j < number_of_functions; ++j)
				{
					ifs >> key >> func_key;
					input_callback* ic = new input_callback(controller_id, pfunctions_to_inputs.find(func_key)->second, func_key);
					Kb::register_button(key, ic);
				}
				break;
			// 1 for Joystick input
			case 1:
				//Js::register_motion(controller_id, rotate);
				for(int j = 0; j < number_of_functions; ++j)
				{
					ifs >> key >> func_key;
					input_callback* ic = new input_callback(controller_id, pfunctions_to_inputs.find(func_key)->second, func_key);
					Js::register_button(key, ic);
				}
				break;
		}

	}

	ifs.close();
}

void
initialize()
{
	int i;
	std::function<void (int, float)> funcs[] = {game_pause, shoot, move_up, move_down,
	move_left, move_right, move_spinl,
	move_spinr, move_forward, move_backward};
	for(i = 0; i < QTTY; ++i)
		pfunctions_to_inputs[i] = funcs[i];

	read_controllers_settings();
}

bool
handle()
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
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			run = Kb::button_event(e);
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

}

