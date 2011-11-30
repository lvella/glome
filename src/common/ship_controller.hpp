#pragma once

#include "vector4.hpp"

struct _ship_engine
{
	float max_rot_per_frame; /* Maximum turning delta per frame */
	float max_speed_forward;
	float max_accel_forward;
	float max_speed_vertical;
	float max_speed_horizontal;
	float max_speed_spin;
};

typedef struct _ship_engine ship_engine;

struct _ship_gun
{
	float shot_speed; /* Speed of shot relative to the ship */
	int shot_power; /* Damage done and heat generated */
	int max_canon_heat; /* Maximum heat supported by canons */
	int canon_cooldown_rate; /* Heat cooldown per frame */
	int cold_fire_rate; /* Shots per second when cold */
};

typedef struct _ship_gun ship_gun;

class ship_controller
{
public:
	ship_controller(ship_engine& engine, int nguns, ship_gun& gunl, ship_gun& gunr);

	void update(Matrix4& t);

	inline void rotate(float rx, float ry)
	{
		h_req = rx / 100.0;
		v_req = -ry / 100.0;
	}

	inline void move_v(float y)
	{
		speed_v = y * engine.max_speed_vertical;
	}

	inline void move_h(float x)
	{
		speed_h = x * engine.max_speed_horizontal;
	}

	inline void move(float a)
	{
		accel = a * engine.max_accel_forward;
	}

	inline void move_s(float a)
	{
		speed_s = a * engine.max_speed_spin;
	}

	inline void shot(bool s)
	{
		shot = s;
	}

private:
	ship_engine& engine;
	int nguns;
	ship_gun& gun_l;
	ship_gun& gun_r;

	/** Shot */
	int shot_count;
	bool canon_shot_last;
	int heat;
	bool shot;

	/** Movement */
	float v_req, h_req;
	float v_tilt, h_tilt;
	float accel;
	float speed, speed_v, speed_h, speed_s;

};

