#pragma once

#include <AL/al.h>

namespace Audio {
   class Source;

   /** Sound samples origin. */
   class Sound {
   public:
      virtual ~Sound() = default;

   protected:
      friend class Source;

      /** Play this sound on a OpenAL source. */
      virtual void selfPlay(ALuint src, bool loop, float offset=0) = 0;
   };

}
