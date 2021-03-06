#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

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
std::vector<std::shared_ptr<ShipController>> ship_controllers;

static void resize_ship_controller(size_t minimum_size)
{
	static std::shared_ptr<ShipController> dummy = std::make_shared<ShipController>();

	// We currently support only 4 players:
	assert(minimum_size <= 4);

	ship_controllers.reserve(minimum_size);
	while(ship_controllers.size() < minimum_size) {
		ship_controllers.emplace_back(dummy);
	}
}

void register_ship_controller(size_t controller_id, std::shared_ptr<ShipController>& sc)
{
	resize_ship_controller(controller_id + 1);
	ship_controllers[controller_id] = sc;
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

		resize_ship_controller(controller_id + 1);

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
	for(i = 0; i < size_t(Event::QTTY); ++i)
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
	switch(Event(e))
	{
	case Event::SHOOT:
	case Event::MOVE_BACKWARD:
	case Event::MOVE_RIGHT:
	case Event::MOVE_UP:
	case Event::MOVE_SPINL:
		return 1.0;
	case Event::MOVE_DOWN:
	case Event::MOVE_LEFT:
	case Event::MOVE_SPINR:
	case Event::MOVE_FORWARD:
		return -1.0;
	case Event::PAUSE:
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
