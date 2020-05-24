#pragma once

#include <unordered_map>

#include <SDL.h>

#include "ship_controller.hpp"
#include "events.hpp"
#include "game.hpp"

namespace Input
{
	/*
	* Generic Callback function for input
	* TODO: Make this work for variable number of arguments:
	* http://en.wikipedia.org/wiki/Variadic_templates
	*/
	extern std::unordered_map<int, ShipController*> ship_controllers;

	ShipController* create_ship_controller(int);

	void read_controllers_settings();

	void initialize();

	bool handle();

	inline float normalize_button(int e)
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

	inline void rotate(int controller_id, float rx, float ry)
	{
		ship_controllers[controller_id]->rotate_h(rx);
		ship_controllers[controller_id]->rotate_v(ry);
	}

	inline void move_forward(int controller_id, float a)
	{
		ship_controllers[controller_id]->move(a);
	}

	inline void move_backward(int controller_id, float a)
	{
		ship_controllers[controller_id]->move(a);
	}

	inline void move_up(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_v(a);
	}

	inline void move_down(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_v(a);
	}

	inline void move_left(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_h(a);
	}

	inline void move_right(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_h(a);
	}

	inline void move_spinl(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_s(a);
	}

	inline void move_spinr(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_s(a);
	}

	inline void shoot(int controller_id, float a)
	{
		ship_controllers[controller_id]->shoot((bool)a);
	}

	inline void game_pause(int b, float a)
	{
		static bool paused = false;
		if(a > 0.5f) {
			Game::switch_state(paused ? Game::State::WORLD : Game::State::MENU);
			paused = !paused;

			SDL_SetRelativeMouseMode(SDL_bool(!paused));
		}
	}
}

