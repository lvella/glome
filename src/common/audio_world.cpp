#include "audio_world.hpp"

using namespace Audio;

void World::addListener(Listener *l) {
	listeners.push_back(l);
	for(Source *s :sources) {
		s->listenersAdded();
	}
}

void World::updateFromListener(size_t listener, const Matrix4& prev, const Matrix4& curr) {
	for(Source *s: sources) {
		s->updateFromListener(listener, curr);
	}
}
