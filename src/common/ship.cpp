#include "ship.hpp"

#include <map>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "math.hpp"
#include "projectile.hpp"
#include "config.hpp"

#include "vector4.hpp"
#include "matrix4.hpp"


#include <iostream>

using namespace std;
using namespace Glome;

extern const char* mesh_filename[Mesh::MESH_COUNT];

void
Ship::set_controller(ShipController* pctrl)
{
	ctrl = pctrl;
	ctrl->stats = stats.get();
}

Ship::Ship(Mesh::Types type, ShipStats::shared_ptr sstats):
		fx_engine(0.001f),
		stats(std::move(sstats))
{
	mesh = Mesh::get_mesh(type);
	load_guns(type);
	load_engines(type);

	rel_speed = 0.0f;

	ctrl = NULL;
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
		ret = fread(&gun_pos, sizeof(unsigned int), 1, fd);
		assert(ret == 1);
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
		ret = fread(&engine_pos, sizeof(unsigned int), 1, fd);
		assert(ret == 1);
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
Ship::update(float dt)
{
	#ifdef STATS_TUNING
	static float curr_scale = 1.0;
	if(curr_scale != stats->scale) {
		curr_scale = stats->scale;
		mesh->rescale(curr_scale);
	}
	#endif
	if(ctrl != NULL)
	{
		/* Turning */
		float h = ctrl->h_tilt - ctrl->h_req;
		float v = ctrl->v_tilt - ctrl->v_req;

		/* Limit the turning speed to max rads per second. */
		float max_rot = dt * stats->max_accel_rot;
		if(h > max_rot)
			h = max_rot;
		else if(h < -max_rot)
			h = -max_rot;
		if(v > max_rot)
			v = max_rot;
		else if(v < -max_rot)
			v = -max_rot;

		ctrl->h_tilt -= h;
		ctrl->v_tilt -= v;

		float old_speed = ctrl->speed;
		ctrl->speed += dt * ctrl->accel;
		if(ctrl->speed > 0.0f) {
			ctrl->speed = 0.0f;
			rel_speed = 0.0f;
		} else if(ctrl->speed < -stats->max_speed_forward) {
			ctrl->speed = -stats->max_speed_forward;
			rel_speed = 1.0f;
		} else if(old_speed != ctrl->speed) {
			rel_speed = -ctrl->speed / stats->max_speed_forward;
		}

		/* Shooting */
		if(ctrl->heat > 0)
			ctrl->heat -= dt * stats->canon_cooldown_rate; // Cooldown rate

		float sps = stats->max_fire_rate - ctrl->heat; // Current firerate

		ctrl->shot_countdown -= dt * sps;
		if(ctrl->shot_countdown < 0)
		{
			if(ctrl->shot)
			{
				Projectile::shot(ctrl,
					_t * (ctrl->canon_shot_last ? l_canon : r_canon),
					stats->shot_speed - ctrl->speed
				);

				ctrl->shot_countdown += 60;
				ctrl->heat += stats->shot_power; // Shot heat, could be equivalent to damage
				ctrl->canon_shot_last = !ctrl->canon_shot_last;
			}
			else
				ctrl->shot_countdown = 0;
		}

		_t = _t * zw_matrix(dt * ctrl->speed) * yw_matrix(dt * ctrl->speed_v)
			* xw_matrix(dt * ctrl->speed_h)
			* xy_matrix(dt * ctrl->speed_s) * yz_matrix(dt * ctrl->v_tilt)
			* rotation(-dt * ctrl->h_tilt, 0.0, math::sqrt1_2, math::sqrt1_2);

		fx_engine.setIntensity(std::max(0.0f, rel_speed));
	}
	fx_engine.update(dt);

}
