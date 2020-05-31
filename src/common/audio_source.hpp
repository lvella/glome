#pragma once

#include <vector>
#include <memory>

#include <AL/al.h>

#include "vector3.hpp"
#include "audio_sound.hpp"
#include "object.hpp"

namespace Audio {
   class Sound;
   class World;

   /** Sound source in the 3-sphere. */
   class Source: virtual public Object {

   public:
      Source();

      /* Non-copyable */
      Source(Source&) = delete;
      Source &operator=(Source&) = delete;

      virtual ~Source() = default;

      /** Plays a sound from this source */
      void play(Sound *sound, bool loop=false, float offset=0);

      void setGain(float g) {
         gain = g;
         setAllProp(AL_GAIN, g);
      }

      void setPitch(float p) {
         pitch = p;
         setAllProp(AL_PITCH, p);
      }

   private:
      void playForListener(Sound *sound, size_t listener, bool loop, float offset);

      struct SourceForListener {
         SourceForListener(Source *s);

         ~SourceForListener() {
            if(al_source)
               alDeleteSources(1, &al_source);
         }

         void update(float dt, Source *s, const QRot& transform);

         Vector3 prev_pos;
         ALuint al_source;
      };

      void setAllProp(ALenum prop, ALfloat val) {
         for(auto& l: listeners_source) {
            alSourcef(l.al_source, prop, val);
         }
      }


      void updateFromListener(float dt, size_t listener, const QRot& curr)
      {
         listeners_source[listener].update(dt, this, curr);
      }

      void setWorld(class World* w);
      void listenersAdded();

      float gain, pitch;
      std::vector<SourceForListener> listeners_source;

      friend class World;
      class World *world = nullptr;
      class Sound *sound_loop = nullptr;
   };
}
