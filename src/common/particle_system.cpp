#include "fire.hpp"

#include "particle_system.hpp"

static Shader program_particle_system;

ParticleSystem::ParticleSystem(int np):
	count(np)
{
	rattrs = new RenderAttributes[np];
	oattrs = new OfflineAttributes[np];
	idx = new uint16_t[np];
	for(uint16_t i = 0; i < np; ++i)
	{
		idx[i] = i;
	}

	glGenBuffers(2, buffobjs);
}

ParticleSystem::~ParticleSystem()
{
	delete idx;
	delete oattrs;
	delete rattrs;

	glDeleteBuffers(2, buffobjs);
}

void ParticleSystem::depthSort(const Matrix4 &t)
{
	// TODO: Do not use hardcoded values, take them from the projection matrix;
	// TODO: Pass the whole camera as parameter.
	float zNear = 0.001f;
	float zFar = 5.0f;
	float dif = zNear - zFar;

	for(int i = 0; i < count; ++i) {
		Vector4 v = t * rattrs[i].position;
		float z = v.z / (1.0 - v.w);

		z = (z * (zFar + zNear) / dif + 2.0 * zFar * zNear / dif) / -z;
		oattrs[i].cam_dist = z;
	}

	struct {
		OfflineAttributes *oattrs;
		bool operator() (uint16_t a, uint16_t b) {
			if(oattrs[a].active != oattrs[b].active)
				return oattrs[a].active; // Enabled comes first

			// Farther comes first
			return oattrs[a].cam_dist > oattrs[b].cam_dist;
		}
	} comparer;

	comparer.oattrs = oattrs;
	std::sort(idx, idx + count, comparer);
}

void ParticleSystem::initialize()
{
	Fire::initialize();
}
