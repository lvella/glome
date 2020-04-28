#pragma once

#include <memory>

#include "config.hpp"

#ifdef STATS_TUNING
#define STATS_VOLATILE volatile
#define STATS_PACKING __attribute__ ((__packed__))
#else
#define STATS_VOLATILE
#define STATS_PACKING
#endif

struct STATS_PACKING ShipStats {
	typedef std::shared_ptr<ShipStats> shared_ptr;
	static shared_ptr get();

	STATS_VOLATILE float scale;

	STATS_VOLATILE float rot_factor;
	STATS_VOLATILE float max_accel_rot;

	STATS_VOLATILE float max_speed_forward;
	STATS_VOLATILE float max_accel_forward;

	STATS_VOLATILE float max_speed_vertical;
	STATS_VOLATILE float max_speed_horizontal;
	STATS_VOLATILE float max_speed_spin;

	STATS_VOLATILE float shot_speed; /* Speed of shot relative to the ship */

	STATS_VOLATILE float canon_cooldown_rate; /* Heat cooldown per frame */
	STATS_VOLATILE float canon_fire_interval;
	STATS_VOLATILE float max_heat; /* Can't fire when above this hot */
	STATS_VOLATILE float shot_power; /* Damage done and heat generated */
};
