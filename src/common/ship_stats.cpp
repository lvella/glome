#include "ship_stats.hpp"

#ifdef STATS_TUNNING
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

static void deleter(ShipStats *del_ptr) {
	munmap(del_ptr, sizeof(ShipStats));
};

#endif

auto ShipStats::get() -> unique_ptr
{
	#ifdef STATS_TUNNING
	int fd = open("ship_params.bin", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(fd, sizeof(ShipStats));

	void *ptr = mmap(NULL, sizeof(ShipStats),
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	unique_ptr ret((ShipStats*)ptr, deleter);

	#else

	unique_ptr ret(new ShipStats);

	#endif

	ret->max_rot_per_frame = 0.03;
	ret->max_speed_forward = 0.0015;
	ret->max_accel_forward = 0.0002;
	ret->max_speed_vertical = 0.0004;
	ret->max_speed_horizontal = 0.0004;
	ret->max_speed_spin = 0.02;

	ret->shot_speed = 0.02;
	ret->shot_power = 82;
	ret->canon_cooldown_rate = 7;
	ret->max_fire_rate = 15;

	return ret;
}
