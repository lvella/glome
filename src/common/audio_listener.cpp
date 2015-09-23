#include "audio_listener.hpp"

using namespace Audio;

Listener::Listener(World* w):
	world(w)
{
	pos_in_world = w->listeners.size();
	w->addListener(this);
}

Listener::Listener(Listener&& other) {
	*this = other;
	world->listeners[pos_in_world] = this;
}

void Listener::update(const Matrix4& transform)
{
	world->updateFromListener(pos_in_world, transform);
}

