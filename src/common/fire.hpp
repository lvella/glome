#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(){}
	~Fire(){}
	Fire(int number_of_particles);
	static void initialize();
	void draw(Matrix4 cam, Matrix4 proj);
};




