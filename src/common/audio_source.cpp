#include "audio_source.hpp"

#include <cassert>
#include "audio_listener.hpp"

using namespace Audio;

Source::Source():
   gain(1.0),
   pitch(1.0)
{}

void
Source::play(Sound *sound, bool loop, float offset)
{
   if(loop) {
      sound_loop = sound;
   }
   assert(!world || listeners_source.size() == world->listeners.size());

   for(size_t i = 0; i < listeners_source.size(); ++i) {
      playForListener(sound, i, loop, offset);
   }
}

void Source::playForListener(Sound *sound, size_t listener, bool loop, float offset)
{
   auto &s = listeners_source[listener];
   auto &l = world->listeners[listener];

   // Before actually playing, reset the position and velocity
   // of the source, to avoid artifacts from bad initialization
   s.prev_pos = get_world_pos();
   s.update(1.0, this, l->transformation());

   sound->selfPlay(s.al_source, loop, offset);
}

void Source::listenersAdded()
{
   assert(world->listeners.size() >= listeners_source.size());
   for(size_t i = listeners_source.size(); i < world->listeners.size(); ++i) {
      listeners_source.emplace_back(this);
      if(sound_loop) {
	 //TODO: find a way to sync with other listeners.
	 // probably impossible, as each listener hears in
	 // its own speed due to doppler effect.
	 playForListener(sound_loop, listeners_source.size()-1, true, 0);
      }
   }
}

void Source::setWorld(World* w)
{
   listeners_source.clear();

   world = w;
   listeners_source.reserve(world->listeners.size());
   for(auto l: world->listeners) {
      listeners_source.emplace_back(this);
      if(sound_loop) {
	 playForListener(sound_loop, listeners_source.size()-1, true, 0);
      }
   }
}

Source::SourceForListener::SourceForListener(Source *s)
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

   alSourcef(al_source, AL_REFERENCE_DISTANCE, 0.01f);
   alSourcef(al_source, AL_MAX_DISTANCE, 2.0f);
}

void Source::SourceForListener::update(float dt, Source *s, const QRot& transform)
{
   Vector3 pos = (transform * s->get_world_pos()).stereo_proj();
   Vector3 velo = (pos - prev_pos) * (1.0f/dt);

   alSource3f(al_source, AL_VELOCITY, velo.x, velo.y, velo.z);
   alSource3f(al_source, AL_POSITION, pos.x, pos.y, pos.z);

   prev_pos = pos;
}
