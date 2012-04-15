#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(int number_of_particles, Matrix4 velocity);
	virtual ~Fire(){}

	void update();
	void draw(Camera& c);

	static void initialize();

	// Little hackish: must be called if viewport ever happens to change size.
	static void set_width(int w);
	static int width;
};

