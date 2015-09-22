#pragma once

#include <vector>
#include <AL/al.h>
#include "vector4.hpp"
#include "vector3.hpp"
#include "audio_sound.hpp"
#include "audio_world.hpp"

namespace Audio {
   class Sound;
   class World;

   /** Sound source in the 3-sphere. */
   class Source {

   public:
      Source(World *w, Vector4& initial_pos);
      
      /* Non-copyable */
      Source(Source&) = delete;
      Source &operator=(Source&) = delete;

      /* But movable */
      Source(Source &&other);
      Source &operator=(Source&&);

      virtual ~Source();

      /** Plays a sound from this source */
      void play(Sound &sound);

      void setGain(float g) {
         gain = g;
         setAllProp(AL_GAIN, g);
      }

      void setPitch(float p) {
         pitch = p;
         setAllProp(AL_PITCH, p);
      }

   private:
      struct SourceForListener {
         SourceForListener(Source *s, const Vector4& initial_pos):
            prev_pos(initial_pos)
         {
            al_source = 0;
            alGenSources(1, &al_source);
            /*if(alGetError() != AL_NO_ERROR) {
               // TODO: Log this in DEBUG mode.
               al_source = 0;
               return;
            }*/
            if(!al_source)
               return;

            // Relative sound position will be calculated in software...
            alSourcei(al_source, AL_SOURCE_RELATIVE, AL_FALSE);
            alSourcef(al_source, AL_PITCH, s->pitch);
            alSourcef(al_source, AL_GAIN, s->gain);
            alSource3f(al_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            alSource3f(al_source, AL_POSITION, prev_pos.x, prev_pos.y, prev_pos.z);
         }

         ~SourceForListener() {
            if(al_source)
               alDeleteSources(1, &al_source);
         }

         void update(Source *s, const Matrix4& curr) {
            Vector3 pos = (curr * s->position()).stereo_proj();
            Vector3 velo = pos - prev_pos;

            alSource3f(al_source, AL_VELOCITY, velo.x, velo.y, velo.z);
            alSource3f(al_source, AL_POSITION, pos.x, pos.y, pos.z);

            prev_pos = pos;
         }

         Vector3 prev_pos;
         ALuint al_source;
      };

      virtual const Vector4 position() const = 0;

      void setAllProp(ALenum prop, ALfloat val) {
         for(auto& l: listeners_source) {
            alSourcef(l.al_source, prop, val);
         }
      }

      void updateFromListener(size_t listener, const Matrix4& curr)
      {
         listeners_source[listener].update(this, curr);
      }

      void listenersAdded();

      float gain, pitch;
      World *world;
      std::vector<SourceForListener> listeners_source;

      friend class World;
   };
}
