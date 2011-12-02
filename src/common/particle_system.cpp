#include "particle_system.hpp"

static Shader program_particle_system;
static GLuint vbo;

ParticleSystem::ParticleSystem(int np):
	number_of_particles(np),
	particle_vector(np)
{}

ParticleSystem::~ParticleSystem()
{}

void
ParticleSystem::draw(Matrix4 camera, Matrix4 projection)
{}

void ParticleSystem::initialize()
{}
