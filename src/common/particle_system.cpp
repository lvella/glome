#include "fire.hpp"

#include "particle_system.hpp"

static Shader program_particle_system;

ParticleSystem::ParticleSystem(int np):
	count(np),
	actives_count(0)
{
	rattrs = new RenderAttributes[np];
	oattrs = new OfflineAttributes[np];
	idx = new uint16_t[np];
	for(uint16_t i = 0; i < np; ++i)
	{
		idx[i] = i;
	}

	glGenBuffers(2, buffobjs);

	// Initializing buffers to maximum size:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(RenderAttributes) * count,
		rattrs, GL_STREAM_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(uint16_t) * count,
		idx, GL_STREAM_DRAW
	);
}

ParticleSystem::~ParticleSystem()
{
	delete[] idx;
	delete[] oattrs;
	delete[] rattrs;

	glDeleteBuffers(2, buffobjs);
}

void ParticleSystem::depthSort(const QRot &t)
{
	const Matrix4& proj = CamShader::getProjection();

	for(int i = 0; i < count; ++i) {
		if(oattrs[i].active) {
			// I like to believe the compiler will optimize the
			// stuff bellow and only perform the needed
			// computation, like with lazy evaluation.
			oattrs[i].cam_dist = (proj * (t * rattrs[i].position).stereo_proj()).z;
		}
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
