#pragma once

#include "object.hpp"
#include "mesh.hpp"
#include "particle_system.hpp"

class Engine: public Object
{
public:
  Engine(MeshTypes type);
  ~Engine();
  void draw(const Shader& cam);
  void draw(Matrix4 cam, Matrix4 proj);
  void load_location(MeshTypes type);

private:
  ParticleSystem* FX_engine;
  Matrix4 velocity;
  uint16_t nengines;

  float max_rot_per_frame; /* Maximum turning delta per frame */
	float max_speed_forward;
	float max_accel_forward;
	float max_speed_vertical;
  float max_speed_horizontal;
  float max_speed_spin;
};

