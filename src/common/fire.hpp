#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(){}
	Fire(int number_of_particles, Matrix4 velocity);
	virtual ~Fire(){}

	void draw(Matrix4 cam, Matrix4 proj);

	static void initialize();
};




