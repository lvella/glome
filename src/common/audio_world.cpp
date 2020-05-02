#include "audio_world.hpp"

using namespace Audio;

void World::addListener(Listener *l) {
	listeners.push_back(l);
	for(Source *s :sources) {
		s->listenersAdded();
	}
}

void World::updateFromListener(size_t listener, const QRot& transform) {
	for(Source *s: sources) {
		s->updateFromListener(listener, transform);
	}
}
