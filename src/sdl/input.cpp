#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <string>

#include <SDL.h>

#include "config.hpp"
#include "input.hpp"
#include "kbinput.hpp"
//#include "jsinput.hpp"

namespace Input
{

/*
* Map function names to functions poiters in controller settings file
*/
std::unordered_map<std::string, Input::pfunction> pfunctions_to_inputs;

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
*/
void read_controllers_settings()
{
	std::ifstream ifs;
	int number_of_controllers, number_of_functions, controller_id, key, input_type;
	std::string func_key;

	std::stringstream dir;
	dir << DATA_DIR << "/controllers/input.config";
	ifs.open(dir.str().c_str(), std::ifstream::in);
	assert(ifs.is_open());
	ifs >> number_of_controllers >> number_of_functions;
	for(int i = 0; i < number_of_controllers; ++i)
	{
		ifs >> input_type >> controller_id;

		std::pair <int, ShipController* > temp;
		temp.first = controller_id;
		temp.second = new ShipController();
		ship_controllers.insert(temp);
		
		for(int j = 0; j < number_of_functions; ++j)
		{
			ifs >> key >> func_key;
			std::pair<int, Input::pfunction> pfunction_pair = std::make_pair(controller_id, pfunctions_to_inputs.find(func_key)->second);
			switch(input_type)
			{
				// 0 for Keyboard input
				case 0:
					Kb::register_input(key, pfunction_pair);
					break;
				// 1 for Joystick input
				case 1:
					break;
			}
		}
	}
  ifs.close();
}

void
initialize()
{
	pfunctions_to_inputs.insert(std::make_pair("quit",Input::quit));
	pfunctions_to_inputs.insert(std::make_pair("shoot",Input::shoot));
	pfunctions_to_inputs.insert(std::make_pair("move_up",Input::move_up));
	pfunctions_to_inputs.insert(std::make_pair("move_down",Input::move_down));
	pfunctions_to_inputs.insert(std::make_pair("move_left",Input::move_left));
	pfunctions_to_inputs.insert(std::make_pair("move_right",Input::move_right));
	pfunctions_to_inputs.insert(std::make_pair("move_spinl",Input::move_spinl));
	pfunctions_to_inputs.insert(std::make_pair("move_spinr",Input::move_spinr));
	pfunctions_to_inputs.insert(std::make_pair("move_forward",Input::move_forward));
	pfunctions_to_inputs.insert(std::make_pair("move_backward",Input::move_backward));

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
        //Kb::mouse_motion(e.motion.x, e.motion.y);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        Kb::mouse_button(e.button.button, e.button.state);
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        run = Kb::key_event(e);
        break;
      case SDL_JOYAXISMOTION:
        //Js::axis_event(e.jaxis);
        break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        //Js::button_event(e.jbutton);
        break;
    }
  }

  return run;
}

}
