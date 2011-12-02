#pragma once

#include "particle_system.hpp"

class Fire : public ParticleSystem
{
public:
	Fire(){}
  virtual ~Fire(){}
	Fire(int number_of_particles, Matrix4 velocity);
	static void initialize();
	void draw(Matrix4 cam, Matrix4 proj);
};




