#include "particle_system.hpp"

static Shader program_particle_system;
static GLuint vbo;

ParticleSystem::ParticleSystem(int np):
	number_of_particles(np),
	particle_vector(np)
{
}

void ParticleSystem::initialize()
{
	
}
