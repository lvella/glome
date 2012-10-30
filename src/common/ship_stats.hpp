#pragma once

#include <memory>

#include "config.hpp"

#ifdef STATS_TUNING
#define STATS_VOLATILE volatile
#define STATS_PACKING __attribute__ ((__packed__))
#define STATS_POINTER std::unique_ptr< ShipStats, void(*)(ShipStats *) >
#else
#define STATS_VOLATILE
#define STATS_PACKING
#define STATS_POINTER std::unique_ptr< ShipStats >
#endif

struct STATS_PACKING ShipStats {
	typedef STATS_POINTER unique_ptr;
	static unique_ptr get();

	STATS_VOLATILE float scale;

	STATS_VOLATILE float max_rot_per_frame; /* Maximum turning delta per frame */
	STATS_VOLATILE float max_speed_forward;
	STATS_VOLATILE float max_accel_forward;
	STATS_VOLATILE float max_speed_vertical;
	STATS_VOLATILE float max_speed_horizontal;
	STATS_VOLATILE float max_speed_spin;

	STATS_VOLATILE float shot_speed; /* Speed of shot relative to the ship */
	STATS_VOLATILE int shot_power; /* Damage done and heat generated */
	STATS_VOLATILE int canon_cooldown_rate; /* Heat cooldown per frame */
	STATS_VOLATILE int max_fire_rate; /* Shots per second when cold */
};
