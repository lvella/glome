#include <memory>

#include "config.hpp"

#ifdef STATUS_TUNNING
#define STATUS_VOLATILE volatile
#define STATUS_PACKING __attribute__ ((__packed__))
#else
#define STATUS_VOLATILE
#define STATUS_PACKING
#endif

struct STATUS_PACKING ShipStats {
	static std::unique_ptr<ShipStats> get();

	STATUS_VOLATILE float scale;

	STATUS_VOLATILE float max_rot_per_frame; /* Maximum turning delta per frame */
	STATUS_VOLATILE float max_speed_forward;
	STATUS_VOLATILE float max_accel_forward;
	STATUS_VOLATILE float max_speed_vertical;
	STATUS_VOLATILE float max_speed_horizontal;
	STATUS_VOLATILE float max_speed_spin;

	STATUS_VOLATILE float shot_speed; /* Speed of shot relative to the ship */
	STATUS_VOLATILE int shot_power; /* Damage done and heat generated */
	STATUS_VOLATILE int canon_cooldown_rate; /* Heat cooldown per frame */
	STATUS_VOLATILE int max_fire_rate; /* Shots per second when cold */
};
