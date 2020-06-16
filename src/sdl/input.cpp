#include <iostream>
#include <fstream>
#include <utility>
#include <string>

#include <assert.h>
#include <SDL.h>

#include "data_file.hpp"
#include "input.hpp"
#include "kbinput.hpp"
#include "jsinput.hpp"
#include "events.hpp"

extern bool is_paused;

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
	int number_of_controllers, number_of_functions, controller_id, key, input_type;
	int func_key;

	std::ifstream ifs = load_data_file("controllers/input.config");
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

bool
handle_paused()
{
	assert(is_paused);

	SDL_Event e;
	while(SDL_WaitEvent(&e))
	{
		switch(e.type)
		{
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if(e.key.keysym.sym == SDLK_ESCAPE) {
				game_pause(0, 1.0f);
				return true;
			}
			if(e.key.keysym.sym == SDLK_q) {
				return false;
			}
		}
	}

	std::cout << "Error on wait event: " << SDL_GetError() << std::endl;
	assert(0);
	return false;
}

float normalize_button(int e)
{
	switch(e)
	{
	case SHOOT:
	case MOVE_BACKWARD:
	case MOVE_RIGHT:
	case MOVE_UP:
	case MOVE_SPINL:
		return 1.0;
	case MOVE_DOWN:
	case MOVE_LEFT:
	case MOVE_SPINR:
	case MOVE_FORWARD:
		return -1.0;
	case PAUSE:
		return 1.0;
	default:
		return 0.0;
	}
}

void game_pause(int b, float a)
{
	if(a > 0.5f) {
		//Game::switch_state(paused ? Game::WORLD : Game::MENU);
		is_paused = !is_paused;

		SDL_SetRelativeMouseMode(SDL_bool(!is_paused));
	}
}

}
