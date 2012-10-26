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

#include "vector4.hpp"
#include "matrix4.hpp"

#include "ship.hpp"

#include <iostream>

using namespace std;
using namespace Glome;

extern const char* mesh_filename[Mesh::MESH_COUNT];

void
Ship::set_controller(ShipController* pctrl)
{
	ctrl = pctrl;
	ctrl->max_rot_per_frame = 0.03;
	ctrl->max_speed_forward = 0.0015;
	ctrl->max_accel_forward = 0.0002;
	ctrl->max_speed_vertical = 0.0004;
	ctrl->max_speed_horizontal = 0.0004;
	ctrl->max_speed_spin = 0.02;
}

Ship::Ship(Mesh::Types type):
		fx_engine(0.001f)
{
	mesh = Mesh::get_mesh(type);
	load_guns(type);
	load_engines(type);

	rel_speed = 0.0f;

	ctrl = NULL;

	shot_speed = shot_speed = 0.02;
	shot_power = shot_power = 82;
	// max_canon_heat = ?; // TODO: Mathematical model for cooldown, that uses all heat parameters.
	canon_cooldown_rate = canon_cooldown_rate = 7;
	cold_fire_rate = cold_fire_rate = 15;
}

void 
Ship::load_guns(Mesh::Types type)
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
		/* Pointer file to gun position */
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
Ship::load_engines(Mesh::Types type)
{
	int ret;
	FILE *fd;

	const char* name = mesh_filename[int(type)];
	// Load .gun file
	{
		unsigned int engine_pos;
		std::stringstream dir;
		dir << DATA_DIR << "/models/" << name << ".wire";
		fd = fopen(dir.str().c_str(), "rb");
		/* Read header of file */
		fseek(fd, 8, SEEK_SET);
		fread(&engine_pos, sizeof(unsigned int), 1, fd);
		/* Poiter file to engine position */
		fseek(fd, engine_pos, SEEK_SET);
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
		/* Turning */
		float h = ctrl->h_tilt - ctrl->h_req;
		float v = ctrl->v_tilt - ctrl->v_req;

		/* Limit the turning speed to MAXD rads per frame. */
		if(h > ctrl->max_rot_per_frame)
			h = ctrl->max_rot_per_frame;
		else if(h < -ctrl->max_rot_per_frame)
			h = -ctrl->max_rot_per_frame;
		if(v > ctrl->max_rot_per_frame)
			v = ctrl->max_rot_per_frame;
		else if(v < -ctrl->max_rot_per_frame)
			v = -ctrl->max_rot_per_frame;

		ctrl->h_tilt -= h;
		ctrl->v_tilt -= v;

		float old_speed = ctrl->speed;
		ctrl->speed += ctrl->accel;
		if(ctrl->speed > ctrl->max_speed_forward) {
			ctrl->speed = ctrl->max_speed_forward;
			rel_speed = -1.0f;
		} else if(ctrl->speed < -ctrl->max_speed_forward) {
			ctrl->speed = -ctrl->max_speed_forward;
			rel_speed = 1.0f;
		} else if(old_speed != ctrl->speed) {
			rel_speed = -ctrl->speed / ctrl->max_speed_forward;
		}

		/* Shooting */
		if(ctrl->heat > 0)
			ctrl->heat -= canon_cooldown_rate; // Cooldown rate

		int sps = (cold_fire_rate * 100 - ctrl->heat) / 100; // Firerate at maximum

		ctrl->shot_count -= sps;
		if(ctrl->shot_count < 0)
		{
			if(ctrl->shot)
			{
				//TODO: Put shots collision elsewhere, rather than the ship_controller
				Projectile::shot(ctrl, _t * (ctrl->canon_shot_last ? l_canon : r_canon), shot_speed - ctrl->speed);
				ctrl->shot_count += 60;
				ctrl->heat += shot_power; // Shot heat, could be equivalent to damage
				ctrl->canon_shot_last = !ctrl->canon_shot_last;
			}
			else
				ctrl->shot_count = 0;
		}

		_t = _t * zw_matrix(ctrl->speed) * yw_matrix(ctrl->speed_v) * xw_matrix(ctrl->speed_h)
			 * xy_matrix(ctrl->speed_s) * yz_matrix(ctrl->v_tilt)
			 * rotation(-ctrl->h_tilt, 0.0, M_SQRT2 / 2.0, M_SQRT2 / 2.0);


		fx_engine.setIntensity(std::max(0.0f, rel_speed));
	}
	fx_engine.update();

}
