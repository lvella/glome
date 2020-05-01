#pragma once

#include <vector>
#include "shader.hpp"
#include "object.hpp"
#include "gl.hpp"
#include "matrix4.hpp"

/*** PARTICLE SYSTEM ***/
/*                      (You don't say?)
                          . ,MMMMMMM=..
                          MMMMMMMMMMMMMM7.
                        .M..    . .:MMMMMMM
                        D.          ..DMMMMM
                       I~.             .NMMMM
                      .  ?M.            .NMMMM
                     .  ~..       .      ~MMMMM
                     8?DM         IM.    MMMMMMM.
                     ...? : ...Z+~  ?   :MMMMMMM
                   .   ...     ~M $.    MMMMMMMMM
                      .,.    .  ..     .MMMMMMMMM
                   , .~:.  .            MMMMMMMMM
                   =     := .          NMM=MMMMM,
                 . I:....     .          .:MMMMI.
          ....,OMMMM  M I...  .         . $MMMM.
       . .OMMMMMMMMM   MMN~~...      . :$7.MMMO~  .
    ..,MMMMMMMMMMMMM   . ..          ... MMMMMMMMMMMM$..
  .?MMMMMMMMMMMMMMMM..            . .  MMMMMMMMMMMMMMMMMZ.
 MMMMMMMMMMMMMMMMMMM .Z         . ~.  MMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMM.         . I   Z8MMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMM: ..  .  .    ..MMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMMM. ~..      7. =MMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMMM7..~.   :.   .MMMMMMMMMMMMMMMMMMMMMMMM*/

class ParticleSystem
{
public:
	ParticleSystem(int np);
	virtual ~ParticleSystem();
	void depthSort(const QRot &t);

	static void initialize();
protected:
	// TODO: Think of some kind of BO pool to share the usage of stream BOs
	union {
		struct {
			GLuint vbo;
			GLuint ibo;
		};
		GLuint buffobjs[2];
	};

	// Attributes sent to OpenGL
	struct RenderAttributes
	{
		Vector4 position;
		Vector4 color;
		float radius; /* in radians */
	};

	// Logical attributes not used directly in rendering
	struct OfflineAttributes
	{
		OfflineAttributes():
			active(false),
			energy(0.0f),
			cam_dist(0.0f)
		{}

		float energy;	/* Energy of the particle */
		float cam_dist;
		bool active; /* Particle is alive or not */
	};

	RenderAttributes *rattrs;
	OfflineAttributes *oattrs;
	uint16_t *idx;
	uint16_t count;
	uint16_t actives_count;
	GLuint tex_particle;
};



