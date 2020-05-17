#pragma once

#include "qrot.hpp"
#include "audio_world.hpp"

namespace Audio {
	class Listener {
	public:
		Listener(World* w);

		Listener(Listener&) = delete;
		Listener(Listener&& other);

		virtual ~Listener() = default;

		void update(const QRot& transform);

		virtual const QRot &transformation() const = 0;

	private:
		Listener &operator=(Listener&) = default;

		size_t pos_in_world;
		World *world;
	};

}
