#include <stdio.h>
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
    heat(0),
    f(100)
{
	Fire::initialize();
  mesh = Mesh::get_mesh(type);
  load_guns(type);
  //message.push_back(msg_id);

  max_rot_per_frame = 0.03;
  max_speed_forward = 0.0015;
  max_speed_vertical = 0.0004;
  max_speed_horizontal = 0.0004;
  max_speed_spin = 0.02;

  shot_speed = 0.02;
  shot_power = 82;
  // max_canon_heat = ?; // TODO: Mathematical model for cooldown, that uses all heat parameters.
  canon_cooldown_rate = 7;
  cold_fire_rate = 15;
}

Ship::~Ship()
{
  Mesh::release_mesh(mesh);
}

void 
Ship::load_guns(MeshTypes type)
{
  int ret;
  FILE *fd;

  const char* name = mesh_filename[int(type)];
  // Load .gun file
  {
    std::stringstream dir;
    dir << DATA_DIR << "/" << name << ".gun";
    fd = fopen(dir.str().c_str(), "rb");
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
Ship::draw(const Shader& s)
{
  s.setTransform(t);
  mesh->draw(s);
}

void 
Ship::update()
{
  float accel_v = 0.f, accel_h = 0.f, accel_s = 0.f;

  /* Turning */
  float h = h_tilt - h_req;
  float v = v_tilt - v_req;

  /* Limit the turning speed to MAXD rads per frame. */
  if(h > max_rot_per_frame)
    h = max_rot_per_frame;
  else if(h < -max_rot_per_frame)
    h = -max_rot_per_frame;
  if(v > max_rot_per_frame)
    v = max_rot_per_frame;
  else if(v < -max_rot_per_frame)
    v = -max_rot_per_frame;

  h_tilt -= h;
  v_tilt -= v;

  /* Handle input commands */
  handle_commands(up, down, speed_v, accel_v, 0.00004);
  handle_commands(right, left, speed_h, accel_h, 0.00004);
  handle_commands(spinl, spinr, speed_s, accel_s, 0.002);

  /* Accelerating */
  accelerate(speed, accel, max_speed_forward);
  accelerate(speed_v, accel_v, max_speed_vertical);
  accelerate(speed_h, accel_h, max_speed_horizontal);
  accelerate(speed_s, accel_s, max_speed_spin);

  /* Shooting */
  if(heat > 0)
    heat -= canon_cooldown_rate; // Cooldown rate

  int sps = (cold_fire_rate * 100 - heat) / 100; // Firerate at maximum

  shot_count -= sps;
  if(shot_count < 0) 
  {
    if(sh)
    {
      Projectile::shot(this, t * (rcanon_shot_last ? l_canon : r_canon), shot_speed - speed);
      
      shot_count += 60;
      heat += shot_power; // Shot heat, could be equivalent to damage
      rcanon_shot_last = !rcanon_shot_last;
    }
    else
      shot_count = 0;
  }

  t = t * zw_matrix(speed) * yw_matrix(speed_v) * xw_matrix(speed_h) * xy_matrix(speed_s) * yz_matrix(v_tilt) * rotation(-h_tilt, 0.0, M_SQRT2/2.0, M_SQRT2/2.0);
}

void
Ship::accelerate(float& speed, float& accel, float max)
{
  speed += accel;
  if(speed > max)
    speed = max;
  else if(speed < -max)
    speed = -max;
}

/*
const char *
Ship::make_message()
{
  string ret = message;
  message = "";
  return ret.c_str();
}
*/

void
Ship::handle_commands(bool k0, bool k1, float& speed, float& accel, float max_accel)
{
  if(k0 || k1)
  {
    accel = max_accel;
    if(k1)
      accel = -accel;
  }
  else
  {
    if(speed > 0.f)
    {
      accel = -max_accel;
      if((speed + accel) < 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
    else if(speed < 0.f)
    {
      accel = max_accel;
      if((speed + accel) > 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
  }
}

