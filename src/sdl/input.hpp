#pragma once

#include <functional>
#include <unordered_map>

#include "ship_controller.hpp"

namespace Input {
	/* 
	* Generic Callback function for input 
	* TODO: Make this work for variable number of arguments:
	* http://en.wikipedia.org/wiki/Variadic_templates
	*/
	typedef std::function<void (int, float)> pfunction;
	
	extern std::unordered_map<int, ShipController*> ship_controllers;
		
	ShipController* set_input_controller();

	void read_controllers_settings();

	void initialize();

  bool handle();

	inline void rotate_h(int controller_id, float rx)
	{
		ship_controllers[controller_id]->rotate_h(rx);
	}

	inline void rotate_v(int controller_id, float ry)
	{
		ship_controllers[controller_id]->rotate_v(ry);
	}

	inline void move_forward(int controller_id, float a)
	{
		std::cout << "move_foward - controller_id: " << controller_id << " force: " << a << std::endl;
		ship_controllers[controller_id]->move(a);
	}

  inline void move_backward(int controller_id, float a)
  {
		ship_controllers[controller_id]->move(-a);
  }

  inline void move_up(int controller_id, float a)
  {
		std::cout << "move_up - controller_id: " << controller_id << " force: " << a << std::endl;
		ship_controllers[controller_id]->move_v(a);
  }

  inline void move_down(int controller_id, float a)
  {
		std::cout << "move_down - controller_id: " << controller_id << " force: " << a << std::endl;
		ship_controllers[controller_id]->move_v(-a);
  }

  inline void move_left(int controller_id, float a)
  {
		ship_controllers[controller_id]->move_h(-a);
  }

  inline void move_right(int controller_id, float a)
  {
		ship_controllers[controller_id]->move_h(a);
  }

  inline void move_spinl(int controller_id, float a)
  {
		ship_controllers[controller_id]->move_s(-a);
	}

  inline void move_spinr(int controller_id, float a)
  {
		ship_controllers[controller_id]->move_s(a);
  }

  inline void shoot(int controller_id, float a)
  {
		ship_controllers[controller_id]->shoot((bool)a);
  }
	//FIXME: not work for variable number of arguments
  inline void quit(int b, float a)
  {
  }
}
