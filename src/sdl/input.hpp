#pragma once

#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "ship_controller.hpp"
#include "events.hpp"
#include "game.hpp"

namespace Input
{
	extern std::vector<std::shared_ptr<ShipController>> ship_controllers;

	void register_ship_controller(size_t controller_id, std::shared_ptr<ShipController>&);

	void read_controllers_settings();

	void initialize();

	bool handle();
	bool handle_paused();

	float normalize_button(int e);

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

	void game_pause(int b, float a);
}
