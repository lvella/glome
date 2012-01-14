#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(int number_of_particles, Matrix4 velocity);
	virtual ~Fire(){}

	void draw(Camera& c);

	static void initialize();
};

