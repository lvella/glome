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

void World::updateFromListener(float dt, size_t listener, const QRot& transform)
{
	clean_and_for_each_valid(sources, [&] (auto&& ptr) {
		ptr->updateFromListener(dt, listener, transform);
	});
}

void World::try_add_source(const std::shared_ptr<Object>& obj)
{
	auto source = std::dynamic_pointer_cast<Source>(obj);
	if(source) {
		sources.emplace_back(source);
		source->setWorld(this);
	}
}
