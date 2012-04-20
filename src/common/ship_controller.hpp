#pragma once

class Ship;

class ShipController
{
public:
	ShipController();

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
		speed_v = y * max_speed_vertical;
	}

	inline void move_h(float x)
	{
		speed_h = x * max_speed_horizontal;
	}

	inline void move(float a)
	{
		accel = a * max_accel_forward;
	}

	inline void move_s(float a)
	{
		speed_s = a * max_speed_spin;
	}

	inline void shoot(bool s)
	{
		shot = s;
	}

private:
	friend class Ship;

	/** Shot */
	int shot_count;
	bool canon_shot_last;
	int heat;
	bool shot;

	/** Movement */
	float max_rot_per_frame; /* Maximum turning delta per frame */
	float max_speed_forward;
	float max_accel_forward;
	float max_speed_vertical;
	float max_speed_horizontal;
	float max_speed_spin;
	float v_req, h_req;
	float v_tilt, h_tilt;
	float accel;
	float speed, speed_v, speed_h, speed_s;
};

