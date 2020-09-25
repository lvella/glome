#pragma once

#include "ship_stats.hpp"
#include "scorer.hpp"

class Ship;

class ShipController: public Scorer
{
public:
	ShipController();

	inline void rotate_h(float rx)
	{
		h_req = rx * stats->rot_factor;
	}

	inline void rotate_v(float ry)
	{
		v_req = -ry * stats->rot_factor;
	}

	inline void move_v(float y)
	{
		speed_v = y * stats->max_speed_vertical;
	}

	inline void move_h(float x)
	{
		speed_h = x * stats->max_speed_horizontal;
	}

	inline void move(float a)
	{
		accel = a * stats->max_accel_forward;
	}

	inline void move_s(float a)
	{
		speed_s = a * stats->max_speed_spin;
	}

	inline void shoot(bool s)
	{
		shot = s;
	}

private:
	friend class Ship;

	/** Shot */
	float shot_countdown;
	float heat;
	bool canon_shot_last;
	bool shot;

	/** Movement */
	ShipStats *stats;
	float v_req, h_req;
	float v_tilt, h_tilt;
	float accel;
	float speed, speed_v, speed_h, speed_s;
};

