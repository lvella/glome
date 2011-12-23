#pragma once

#include <cstdio>

#include "drawable.hpp"
#include "mesh.hpp"
#include "particle_system.hpp"

class Engine: public drawable::Drawable
{
public:
	Engine(MeshTypes type, fpos_t engine_position_infile);
	Engine(MeshTypes type, fpos_t engine_position_infile, float rot_per_frame, 
			float speed_forward, float accel_forward, float speed_vertical, 
			float speed_horizontal, float max_speed_spin);
	~Engine();
	void draw(const Shader& cam);
	void draw(Matrix4 cam, Matrix4 proj);
	void load_position(MeshTypes type, fpos_t pos_infile);

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

