#include "ship.hpp"

#include <map>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "math.hpp"
#include "projectile.hpp"
#include "data_file.hpp"
#include "vector4.hpp"

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
	stats(std::move(sstats))
{
	mesh = Mesh::get_mesh(type);
	set_radius(mesh->get_radius());

	load_guns(type);
	load_engines(type);

	rel_speed = 0.0f;

	ctrl = NULL;
}


void Ship::create_sub_objects(std::vector<std::weak_ptr<SubObject>>& objs)
{
	fx_engine = std::make_shared<Fire>(weak_from_this(), 0.001f);
	objs.push_back(fx_engine);
}

void
Ship::load_guns(Mesh::Types type)
{
	bool ret;

	const char* name = mesh_filename[int(type)];
	auto fd = load_data_file("models/"s + name + ".wire"s);
	{
		/* Read from file header */
		fd.seekg(4);
		unsigned int gun_pos;
		ret = fd.read_binary(&gun_pos);
		assert(ret);
		/* Pointer file to gun position */
		fd.seekg(gun_pos);
	}

	{
		// Reading Guns Matrix
		ret = fd.read_binary(&nguns);
		assert(ret);

		Matrix4 mat;
		ret = fd.read_binary(&mat);
		assert (ret);
		l_canon = QRot(mat.transpose());

		ret = fd.read_binary(&mat);
		assert (ret);
		r_canon = QRot(mat.transpose());
	}
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

		// This matrix does nothing...
		// TODO: clean this up
		/*
		Matrix4 velocity;
		ret = fread(&velocity[0][0], sizeof(float), 16, fd);
		assert (ret == 16);
		*/
	}
	fclose(fd);
}

void
Ship::draw(Camera& c)
{
	c.setQRot(get_t());
	mesh->draw(c);
}

bool
Ship::update(float dt, UpdatableAdder& adder)
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
		ctrl->heat = std::max(0.0f, ctrl->heat - dt * stats->canon_cooldown_rate);

		if(ctrl->shot_countdown > 0.0) {
			ctrl->shot_countdown -= dt;
		}

		if(ctrl->shot_countdown <= 0.0f)
		{
			if(ctrl->shot && ctrl->heat < stats->max_heat)
			{
				const float speed = stats->shot_speed - ctrl->speed;

				// Adjust bullet position w.r.t. the exact time the bullet
				// should have been shot, but compensate to the first update,
				// bringing the bullet closer to the cannon:
				const float offset = speed * (ctrl->shot_countdown + dt);

				Projectile::shot(ctrl,
					get_t()
					* (ctrl->canon_shot_last ? l_canon : r_canon)
					* zw_qrot(offset), speed
				);

				// Heat generated by shoting, could be proportional to damage:
				ctrl->heat += stats->shot_power;

				ctrl->shot_countdown += stats->canon_fire_interval;
				ctrl->canon_shot_last = !ctrl->canon_shot_last;
			}
			else
				ctrl->shot_countdown = 0.0;
		}

		mul_t(zw_qrot(dt * ctrl->speed) * yw_qrot(dt * ctrl->speed_v)
			* xw_qrot(dt * ctrl->speed_h)
			* xy_qrot(dt * ctrl->speed_s) * yz_qrot(dt * ctrl->v_tilt)
			* turn(-dt * ctrl->h_tilt)
		);

		fx_engine->set_intensity(std::max(0.0f, rel_speed));
	}
	fx_engine->update(dt);

	return true;
}
