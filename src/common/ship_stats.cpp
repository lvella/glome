#include "ship_stats.hpp"

#ifdef STATS_TUNING
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

static void deleter(ShipStats *del_ptr) {
	munmap(del_ptr, sizeof(ShipStats));
};

#endif

auto ShipStats::get() -> shared_ptr
{
	#ifdef STATS_TUNING
	int fd = open("ship_params.bin", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(fd, sizeof(ShipStats));

	void *ptr = mmap(NULL, sizeof(ShipStats),
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	shared_ptr ret((ShipStats*)ptr, deleter);

	#else

	shared_ptr ret(new ShipStats);

	#endif

	ret->scale = 1.0f;

	ret->rot_factor = 0.6; // rad / s
	ret->max_accel_rot = 108.0; // rad / s²

	ret->max_speed_forward = 0.09; // rad / s
	ret->max_accel_forward = 1.44; // rad / s²

	ret->max_speed_vertical = 0.024; // rad / s
	ret->max_speed_horizontal = 0.024; // rad / s
	ret->max_speed_spin = 1.2; // rad / s

	ret->shot_speed = 0.02;
	ret->shot_power = 82;
	ret->canon_cooldown_rate = 4.2;
	ret->max_fire_rate = 900;

	return ret;
}
