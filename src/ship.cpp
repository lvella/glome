#include <stdio.h>
#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include "4dmath.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "protocol.hpp"
#include "config.hpp"

using namespace std;

extern const char* mesh_filename[SHIPMESH_COUNT];

Ship::Ship(ShipMesh type):
    Drawable(Matrix4::IDENTITY),
    v_tilt(0.0f),
    h_tilt(0.0f),
    v_req(0.0f),
    h_req(0.0f),
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
    msg_id(0)
{
  mesh = Mesh::get_mesh(type);
  load_guns(type);
  //message.push_back(msg_id);
}

Ship::~Ship()
{
  Mesh::release_mesh(mesh);
}

void 
Ship::load_guns(ShipMesh type)
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
Ship::draw()
{
  mesh->draw(t);
}

void 
Ship::update()
{
  /* Maximum turning delta per frame in radians. */
  const float MAXR = 0.03;

  /* Maximum speed, in radians per second. */
  const float MAXS = 0.0015;
  const float MAXS_V = 0.0004;
  const float MAXS_H = 0.0004;
  const float MAXS_S = 0.02;

  float accel_v = 0.f, accel_h = 0.f, accel_s = 0.f;

  /* Turning */
  float h = h_tilt - h_req;
  float v = v_tilt - v_req;

  /* Limit the turning speed to MAXD rads per frame. */
  if(h > MAXR)
    h = MAXR;
  else if(h < -MAXR)
    h = -MAXR;
  if(v > MAXR)
    v = MAXR;
  else if(v < -MAXR)
    v = -MAXR;

  h_tilt -= h;
  v_tilt -= v;

  /* Handle input commands */
  handle_commands(up, down, speed_v, accel_v, 0.00004);
  handle_commands(right, left, speed_h, accel_h, 0.00004);
  handle_commands(spinl, spinr, speed_s, accel_s, 0.002);

  /* Accelerating */
  accelerate(speed, accel, MAXS);
  accelerate(speed_v, accel_v, MAXS_V);
  accelerate(speed_h, accel_h, MAXS_H);
  accelerate(speed_s, accel_s, MAXS_S);

  /* Shooting */
  if(heat > 0)
    heat -= 7; // Cooldown rate

  int sps = (1500 - heat) / 100; // Firerate at maximum

  shot_count -= sps;
  if(shot_count < 0) {
    if(sh) {
      Projectile::shot(this, t * (rcanon_shot_last ? l_canon : r_canon), 0.02 - speed);
      shot_count += 60;
      heat += 82; // Shot heat, could be equivalent to damage
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

