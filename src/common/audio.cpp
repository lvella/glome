#include "audio.hpp"

#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#include "initialization.hpp"

static ALCdevice* al_device;
static ALCcontext* al_context;

static RegisterInitialization ini{[]
{
   al_device = alcOpenDevice(nullptr);
   if(al_device == nullptr) {
      std::cerr << "Warning: OpenAL device could not be opened. Audio is disabled!\n";
      return;
   }

   al_context = alcCreateContext(al_device, nullptr);
   alcMakeContextCurrent(al_context);

   // TODO: properly configure AL_SPEED_OF_SOUND
}};

void Audio::shutdown()
{
   if(al_device)
      alcCloseDevice(al_device);

   alcDestroyContext(al_context);
}
