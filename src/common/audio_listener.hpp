#pragma once

#include "matrix4.hpp"
#include "audio_world.hpp"

namespace Audio {
	class World;

	class Listener {
	public:
		Listener(World* w):
			world(w)
		{
			pos_in_world = w->listeners.size();
			w->addListener(this);
		}
		Listener(Listener&) = delete;
		Listener &operator=(Listener&) = delete;

		void update(const Matrix4& transform)
		{
			world->updateFromListener(pos_in_world, transform);
		}

	private:
		size_t pos_in_world;
		World *world;
	};

}
