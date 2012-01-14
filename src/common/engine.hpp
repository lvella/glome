#pragma once

#include <cstdio>

#include "drawable.hpp"
#include "mesh.hpp"
#include "particle_system.hpp"

class Engine: public Glome::Drawable
{
public:
	Engine(MeshTypes type);
	Engine(MeshTypes type, float rot_per_frame,
	float speed_forward, float accel_forward, float speed_vertical,
	float speed_horizontal, float max_speed_spin);
	~Engine();
	void draw(Camera& c);
	void load_location(MeshTypes type);

private:
	ParticleSystem* FX_engine;
	Matrix4 velocity;
	uint16_t nengines;

public:
	float max_rot_per_frame; /* Maximum turning delta per frame */
	float max_speed_forward;
	float max_accel_forward;
	float max_speed_vertical;
	float max_speed_horizontal;
	float max_speed_spin;
};

