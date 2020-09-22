#include "projectile.hpp"
#include "matrix4.hpp"
#include "math.hpp"

#include "ship_controller.hpp"

ShipController::ShipController()
{
	static ShipStats dummy;

	stats = &dummy;

	v_req = h_req = v_tilt = h_tilt = accel = speed = speed_v = speed_h = speed_s = 0.0f;
	shot = false;
	shot_countdown = 0;
	canon_shot_last = false;
	heat = 0;
}
