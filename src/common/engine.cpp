#include <sstream>

#include "engine.hpp"
#include "config.hpp"
#include "fire.hpp"

extern const char* mesh_filename[MESH_COUNT];

Engine::Engine(MeshTypes type, fpos_t engine_position_infile,
	float rot_per_frame, float speed_forward, float accel_forward,
	float speed_vertical, float speed_horizontal, float speed_spin)
{
	max_rot_per_frame = rot_per_frame;
	max_speed_forward = speed_forward;
	max_accel_forward = accel_forward;
	max_speed_vertical = speed_vertical;
	max_speed_horizontal = speed_horizontal;
	max_speed_spin = speed_spin;

	load_position(type, engine_position_infile);
	FX_engine = new Fire(100, velocity);
}

Engine::~Engine()
{
	delete FX_engine;
}

//TODO: make load_origin_matrix generic to get engine position
void 
Engine::load_position(MeshTypes type, fpos_t pos_infile)
{
	int ret;
	FILE *fd;

	const char* name = mesh_filename[int(type)];
	// Load .gun file
	{
		std::stringstream dir;
		dir << DATA_DIR << "/art/" << name << ".wire";
		fd = fopen(dir.str().c_str(), "rb");
		/* Poiter file to engine position */
		fsetpos(fd, &pos_infile);
		assert(fd != NULL);
	}

	{
		// Reading Engine Matrix
		ret = fread(&nengines, sizeof(nengines), 1, fd);
		assert(ret == 1);

		ret = fread(&velocity[0][0], sizeof(float), 16, fd);
		assert (ret == 16);
	}
	fclose(fd);
}

void Engine::draw(const Shader& cam)
{
}

void Engine::draw(Matrix4 cam, Matrix4 proj)
{
	FX_engine->draw(cam,proj);
}

