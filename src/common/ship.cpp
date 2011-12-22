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

extern const char* mesh_filename[MESH_COUNT];

void
Ship::set_controller(ShipController* pctrl)
{
	ctrl = pctrl;
	ctrl->set_engine(engine);
	ctrl->set_guns(&gun_l, &gun_r);
}

Ship::Ship(MeshTypes type)
{
	mesh = Mesh::get_mesh(type);
	load_guns(type, mesh->get_current_pos());
	engine = new Engine(type, mesh->get_current_pos(), 0.03, 0.0015, 0.0015, 0.0004, 0.0004, 0.02);
	ctrl = NULL;

	gun_l.shot_speed = gun_r.shot_speed = 0.02;
	gun_l.shot_power = gun_r.shot_power = 82;
	// max_canon_heat = ?; // TODO: Mathematical model for cooldown, that uses all heat parameters.
	gun_l.canon_cooldown_rate = gun_r.canon_cooldown_rate = 7;
 	gun_l.cold_fire_rate = gun_r.cold_fire_rate = 15;
}

void 
Ship::load_guns(MeshTypes type, fpos_t gun_position_infile)
{
	int ret;
	FILE *fd;

	const char* name = mesh_filename[int(type)];
	// Load .gun file
	{
		std::stringstream dir;
		dir << DATA_DIR << "/art/" << name << ".wire";
		fd = fopen(dir.str().c_str(), "rb");
		/* Poiter file to gun position */
		fsetpos(fd, &gun_position_infile);
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

	/* Update current pointer position of fd */
	fpos_t cpos;
	fgetpos(fd, &cpos);
	mesh->set_current_pos(cpos);

	fclose(fd);
}

void 
Ship::draw(const Shader& s)
{
	s.setTransform(_t);
	mesh->draw(s);
}

void
Ship::draw(const Shader& s, Matrix4 cam, Matrix4 proj)
{
	s.setTransform(_t);
	mesh->draw(s);
	//engine->draw(cam, proj);
}

void
Ship::update()
{
	if(ctrl != NULL)
		ctrl->update(_t);
}

