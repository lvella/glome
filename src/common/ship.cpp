<<<<<<< HEAD
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
=======
#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include "math.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "protocol.hpp"
#include "config.hpp"
#include "effect.hpp"

using namespace std;

extern const char* mesh_filename[MESH_COUNT];

Ship::Ship(MeshTypes type):
    Object(Matrix4::IDENTITY),
    msg_id(0),
    v_req(0.0f),
    h_req(0.0f),
    v_tilt(0.0f),
    h_tilt(0.0f),
    accel(0.0f),
    speed(0.0f),
    speed_v(0.0f),
    speed_h(0.0f),
    speed_s(0.0f),
    forward(false),
    backward(false),
    up(false),
    down(false),
    left(false),
    right(false),
    spinl(false),
    spinr(false),
    sh(false),
    q(false),
    shot_count(0),
    rcanon_shot_last(false),
    heat(0)
{
  mesh = Mesh::get_mesh(type);
  //TODO: create a class gun
  load_guns(type, mesh->get_current_pos());
  engine = new Engine(type, mesh->get_current_pos()); 

  //message.push_back(msg_id);

  max_rot_per_frame = 0.03;
  max_speed_forward = 0.0015;
  max_speed_vertical = 0.0004;
  max_speed_horizontal = 0.0004;
  max_speed_spin = 0.02;

  shot_speed = 0.02;
  shot_power = 82;
  // max_canon_heat = ?; 
  //TODO: Mathematical model for cooldown, that uses all heat parameters.
  canon_cooldown_rate = 7;
  cold_fire_rate = 15;
}

Ship::~Ship()
>>>>>>> a072e8af45b299f75f83e7a0912c78552a7aaafa
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
<<<<<<< HEAD
ship::load_guns(MeshTypes type)
=======
Ship::load_guns(MeshTypes type, fpos_t gun_position_infile)
>>>>>>> a072e8af45b299f75f83e7a0912c78552a7aaafa
{
	int ret;
	FILE *fd;
	Matrix4 lgun, rgun;

<<<<<<< HEAD
	const char* name = mesh_filename[int(type)];
=======
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
>>>>>>> a072e8af45b299f75f83e7a0912c78552a7aaafa

	// Load .gun file
	{
		std::stringstream dir;
		dir << DATA_DIR << "/" << name << ".gun";
		fd = fopen(dir.str().c_str(), "rb");
		assert(fd != NULL);
	}

<<<<<<< HEAD
	// Reading Guns Matrix
	{
		ret = fread(&nguns, sizeof(nguns), 1, fd);
		assert(ret == 1);
=======
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
  s.setTransform(t);
  mesh->draw(s);
	assert(3 == 4);
}

void
Ship::draw(const Shader& s,Matrix4 cam, Matrix4 proj)
{
  s.setTransform(t);
  mesh->draw(s);
  engine->draw(cam,proj);
}


void 
Ship::update()
{
  float accel_v = 0.f, accel_h = 0.f, accel_s = 0.f;
>>>>>>> a072e8af45b299f75f83e7a0912c78552a7aaafa

		ret = fread(&lgun[0][0], sizeof(float), 16, fd);
		assert (ret == 16);

		ret = fread(&rgun[0][0], sizeof(float), 16, fd);
		assert (ret == 16);
	}

	gun_l.set_transformation(lgun);
	gun_r.set_transformation(rgun);

	fclose(fd);
}

