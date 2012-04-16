#pragma once

#include <vector>
#include "shader.hpp"
#include "object.hpp"
#include "textures.hpp"
#include "gl.hpp"
#include "drawable.hpp"
#include "updatable.hpp"

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

class ParticleSystem : public Updatable, public Glome::Drawable
{
public:
	ParticleSystem(int np);
	virtual	~ParticleSystem() = 0;

	static void initialize();
protected:
	struct Particle
	{
		bool active; /* Particle is alive or not */
		int energy;	/* Energy of the particle */
		float radius; /* Radius, in radians */
		Vector4 color;
		Vector4 position;
		Matrix4 velocity; 
		Particle(){}
	};
	std::vector<Particle> particle_vector;
	uint16_t number_of_particles;
	GLuint tex_particle;
};



