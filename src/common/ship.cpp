#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "math.hpp"
#include "projectile.hpp"
#include "config.hpp"
#include "effect.hpp"

#include "ship.hpp"

#include <iostream>

using namespace std;
using namespace Glome;

extern const char* mesh_filename[MESH_COUNT];

void
Ship::set_controller(ShipController* pctrl)
{
	ctrl = pctrl;
	ctrl->set_engine(engine);
	ctrl->set_guns(&gun_l, &gun_r);
	ctrl->set_guns_matrices(l_canon, r_canon);
}

Ship::Ship(MeshTypes type):
		fx_engine(0.001f)
{
	mesh = Mesh::get_mesh(type);
	load_guns(type);
	engine = new Engine(type, 0.03, 0.0015, 0.0002, 0.0004, 0.0004, 0.02);
	ctrl = NULL;

	gun_l.shot_speed = gun_r.shot_speed = 0.02;
	gun_l.shot_power = gun_r.shot_power = 82;
	// max_canon_heat = ?; // TODO: Mathematical model for cooldown, that uses all heat parameters.
	gun_l.canon_cooldown_rate = gun_r.canon_cooldown_rate = 7;
 	gun_l.cold_fire_rate = gun_r.cold_fire_rate = 15;
}

void 
Ship::load_guns(MeshTypes type)
{
	int ret;
	FILE *fd;

	const char* name = mesh_filename[int(type)];
	{
		unsigned int gun_pos;
		std::stringstream dir;
		dir << DATA_DIR << "/models/" << name << ".wire";
		fd = fopen(dir.str().c_str(), "rb");
		/* Read header of file */
		fseek(fd, 4, SEEK_SET);
		fread(&gun_pos, sizeof(unsigned int), 1, fd);
		/* Poiter file to gun position */
		fseek(fd, gun_pos, SEEK_SET);
		assert(fd != NULL);
	}

	{
		// Reading Guns Matrix
		ret = fread(&nguns, sizeof(nguns), 1, fd);
		assert(ret == 1);

		ret = fread(&l_canon[0][0], sizeof(float), 16, fd);
		assert (ret == 16);

		ret = fread(&r_canon[0][0], sizeof(float), 16, fd);
		assert (ret == 16);
	}
	fclose(fd);
}

void 
Ship::draw(Camera& c)
{
	c.pushMultMat(_t);
	mesh->draw(c);
	fx_engine.draw(c);
	c.popMat();
}

void
Ship::update()
{
	if(ctrl != NULL)
	{
		ctrl->update(_t);
	}
	//fx_engine.setIntensity(1.0f);
	fx_engine.update();
}

