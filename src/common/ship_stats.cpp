#include "ship_stats.hpp"

#ifdef STATUS_TUNNING
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

std::unique_ptr< ShipStats > ShipStats::get()
{
	#ifdef STATUS_TUNNING
	int fd = open("ship_params.bin", O_CREAT | O_RDWR);
	ftruncate(fd, sizeof(ShipStats));

	void *ptr = mmap(NULL, sizeof(ShipStats),
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	auto deleter = [=fd](ShipStats *del_ptr) {
		munmap(del_ptr, sizeof(ShipStats));
		
		close(fd);
	}

	std::unique_ptr<ShipStats, decltype(deleter)> ret((ShipStats*)ptr, deleter);

	#else

	std::unique_ptr<ShipStats> ret(new ShipStats);

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