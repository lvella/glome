#include <cassert>
#include "audio_source.hpp"

using namespace Audio;

Source::Source(World *w, Vector4& initial_pos):
   gain(1.0),
   pitch(1.0),
   world(w)
{
   for(auto l: world->listeners) {
      listeners_source.emplace_back(this, initial_pos);
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
Source::play(Sound &sound)
{
}

void Source::listenersAdded()
{
   assert(world->listeners.size() >= listeners_source.size());
   for(size_t i = listeners_source.size(); i < world->listeners.size(); ++i) {
      listeners_source.emplace_back(this, position());
   }
}
