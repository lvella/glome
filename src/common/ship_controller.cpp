#include "projectile.hpp"
#include "matrix4.hpp"
#include "math.hpp"

#include "ship_controller.hpp"

ShipController::ShipController()
{
	v_req = h_req = v_tilt = h_tilt = accel = rel_speed = speed = speed_v = speed_h = speed_s = 0.0f;
	shot_count = 0;
    canon_shot_last = false;
    heat = 0;
	shot = false;
}

void
ShipController::update(Matrix4& t)
{
	/* Turning */
	float h = h_tilt - h_req;
	float v = v_tilt - v_req;

	/* Limit the turning speed to MAXD rads per frame. */
	if(h > engine->max_rot_per_frame)
		h = engine->max_rot_per_frame;
	else if(h < -engine->max_rot_per_frame)
		h = -engine->max_rot_per_frame;
	if(v > engine->max_rot_per_frame)
		v = engine->max_rot_per_frame;
	else if(v < -engine->max_rot_per_frame)
		v = -engine->max_rot_per_frame;

	h_tilt -= h;
	v_tilt -= v;

	float old_speed = speed;
	speed += accel;
	if(speed > engine->max_speed_forward) {
		speed = engine->max_speed_forward;
		rel_speed = -1.0f;
	} else if(speed < -engine->max_speed_forward) {
		speed = -engine->max_speed_forward;
		rel_speed = 1.0f;
	} else if(old_speed != speed) {
		rel_speed = -speed / engine->max_speed_forward;
	}

	/* Shooting */
	if(heat > 0)
		heat -= gun_l->canon_cooldown_rate; // Cooldown rate

	int sps = (gun_l->cold_fire_rate * 100 - heat) / 100; // Firerate at maximum

	shot_count -= sps;
	if(shot_count < 0)
	{
		if(shot)
		{
			Projectile::shot(this, t * (canon_shot_last ? l_canon : r_canon), gun_l->shot_speed - speed);
			shot_count += 60;
			heat += gun_l->shot_power; // Shot heat, could be equivalent to damage
			canon_shot_last = !canon_shot_last;
		}
		else
			shot_count = 0;
	}

	t = t * zw_matrix(speed) * yw_matrix(speed_v) * xw_matrix(speed_h)
		 * xy_matrix(speed_s) * yz_matrix(v_tilt)
		 * rotation(-h_tilt, 0.0, M_SQRT2 / 2.0, M_SQRT2 / 2.0);
}

