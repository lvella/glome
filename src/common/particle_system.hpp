#pragma once

#include <vector>
#include "shader.hpp"
#include "object.hpp"
#include "textures.hpp"
#include "gl.hpp"

/*** PARTICLE SYSTEM ***/

class ParticleSystem : public Object
{
public:
	ParticleSystem(int np);
	ParticleSystem(){}
  virtual	~ParticleSystem() = 0;
	void draw(Matrix4 camera, Matrix4 projection);
	void draw(const Shader& cam);
	static void initialize();
protected:
	struct Particle
	{
		bool active;	/* Particle is alive or not */
		float fade; 	/* Fade Speed */
		int energy;	/*  Energy of the particle */
		float size;
		Vector4 color;	
		Vector4 position;
		//Vector4	oldPos;
		Matrix4 velocity; 
		Particle(){}
	};
	std::vector<Particle> particle_vector;
	uint16_t number_of_particles;
	GLuint tex_particle;
};



