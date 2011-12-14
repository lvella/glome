#include "aship.hpp"

extern const char* mesh_filename[MESH_COUNT];

ship::ship(MeshTypes type):
	v_req(0.0f),
	h_req(0.0f),
	v_tilt(0.0f),
	h_tilt(0.0f),
	accel(0.0f),
	speed(0.0f),
	speed_v(0.0f),
	speed_h(0.0f),
	speed_s(0.0f),
	shot(false),
	shot_count(0),
	nguns(2),
	canon_lost_shot(false),
	heat(0)
{
	mesh = Mesh::get_mesh(type);
	load_guns(type);

	engine.max_rot_per_frame = 0.03;
	engine.max_speed_forward = 0.0015;
	engine.max_accel_forward = 0.0015;
	engine.max_speed_vertical = 0.0004;
	engine.max_speed_horizontal = 0.0004;
	engine.max_speed_spin = 0.02;

	gun_l.shot_speed = gun_r.shot_speed = 0.02;
	gun_l.shot_power = gun_r.shot_power = 82;
	// max_canon_heat = ?; // TODO: Mathematical model for cooldown, that uses all heat parameters.
	gun_l.canon_cooldown_rate = gun_r.canon_cooldown_rate = 7;
 	gun_l.cold_fire_rate = gun_r.cold_fire_rate = 15;
}

void 
ship::load_guns(MeshTypes type)
{
	int ret;
	FILE *fd;
	Matrix4 lgun, rgun;

	const char* name = mesh_filename[int(type)];

	// Load .gun file
	{
		std::stringstream dir;
		dir << DATA_DIR << "/" << name << ".gun";
		fd = fopen(dir.str().c_str(), "rb");
		assert(fd != NULL);
	}

	// Reading Guns Matrix
	{
		ret = fread(&nguns, sizeof(nguns), 1, fd);
		assert(ret == 1);

		ret = fread(&lgun[0][0], sizeof(float), 16, fd);
		assert (ret == 16);

		ret = fread(&rgun[0][0], sizeof(float), 16, fd);
		assert (ret == 16);
	}

	gun_l.set_transformation(lgun);
	gun_r.set_transformation(rgun);

	fclose(fd);
}

