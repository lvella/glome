#pragma once

#include "vector4.hpp"
#include "engine.hpp"

struct ship_gun
{
	float shot_speed; /* Speed of shot relative to the ship */
	int shot_power; /* Damage done and heat generated */
	int max_canon_heat; /* Maximum heat supported by canons */
	int canon_cooldown_rate; /* Heat cooldown per frame */
	int cold_fire_rate; /* Shots per second when cold */
};

class ShipController
{
public:
	ShipController();

	void update(Matrix4& t);

	inline void set_engine(Engine* pEng) { engine = pEng; }

	inline void set_guns(ship_gun* pL, ship_gun* pR) { gun_l = pL; gun_r = pR; }

	inline void set_guns_matrices(Matrix4& l, Matrix4& r) { l_canon = l; r_canon = r; }

	inline void rotate_h(float rx)
	{
		h_req = rx / 100.0;
	}

	inline void rotate_v(float ry)
	{
		v_req = -ry / 100.0;
	}

	inline void move_v(float y)
	{
		speed_v = y * engine->max_speed_vertical;
	}

	inline void move_h(float x)
	{
		speed_h = x * engine->max_speed_horizontal;
	}

	inline void move(float a)
	{
		accel = a * engine->max_accel_forward;
	}

	inline void move_s(float a)
	{
		speed_s = a * engine->max_speed_spin;
	}

	inline void shoot(bool s)
	{
		shot = s;
	}

	inline const Matrix4& transformation() { return _t; }

private:
	Matrix4 _t;
	Engine* engine;
	int nguns;
	ship_gun* gun_l;
	ship_gun* gun_r;
	Matrix4 l_canon;
	Matrix4 r_canon;

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

