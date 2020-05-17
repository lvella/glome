#include "audio_world.hpp"

#include "audio_source.hpp"
#include "utils.hpp"
#include <memory>

using namespace Audio;

void World::addListener(Listener *l)
{
	listeners.push_back(l);
	clean_and_for_each_valid(sources, [] (auto&& ptr) {
		ptr->listenersAdded();
	});
}

void World::updateFromListener(size_t listener, const QRot& transform)
{
	clean_and_for_each_valid(sources, [&] (auto&& ptr) {
		ptr->updateFromListener(listener, transform);
	});
}
