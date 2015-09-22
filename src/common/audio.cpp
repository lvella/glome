#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#include "audio.hpp"

using namespace Audio;

static ALCdevice* al_device;
static ALCcontext* al_context;

void Initialize()
{
   al_device = alcOpenDevice(NULL);
   if(al_device == NULL) {
      std::cerr << "Warning: OpenAL device could not be opened. Audio is disabled!\n";
      return;
   }

   al_context = alcCreateContext(al_device, NULL);
   alcMakeContextCurrent(al_context);
}

void Shutdown()
{
   if(al_device)
      alcCloseDevice(al_device);

   alcDestroyContext(al_context);
}
