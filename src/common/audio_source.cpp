#include <cassert>
#include "audio_source.hpp"

using namespace Audio;

Source::Source(World *w):
   gain(1.0),
   pitch(1.0),
   world(w)
{
   listeners_source.reserve(world->listeners.size());
   for(auto l: world->listeners) {
      listeners_source.emplace_back(this);
   }

   world->sources.insert(this);
}

Source::Source(Source&& other)
{
   *this = std::move(other);
   world->sources.insert(this);
}

Source &
Source::operator=(Source&& o)
{
   gain = o.gain;
   pitch = o.pitch;
   world = o.world;

   listeners_source = std::move(o.listeners_source);

   return *this;
}

Source::~Source()
{
   world->sources.erase(this);
}

void
Source::play(Sound &sound, bool loop, float offset)
{
   assert(listeners_source.size() == world->listeners.size());

   for(size_t i = 0; i < listeners_source.size(); ++i) {
      auto &s = listeners_source[i];
      auto &l = world->listeners[i];

      // Before actually playing, reset the position and velocity
      // of the source, to avoid artifacts from bad initialization
      s.prev_pos = position();
      s.update(this, l->transformation());

      sound.selfPlay(s.al_source, loop, offset);
   }
}

void Source::listenersAdded()
{
   assert(world->listeners.size() >= listeners_source.size());
   for(size_t i = listeners_source.size(); i < world->listeners.size(); ++i) {
      listeners_source.emplace_back(this);
   }
}
