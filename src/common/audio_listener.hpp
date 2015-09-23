#pragma once

#include "matrix4.hpp"
#include "audio_world.hpp"

namespace Audio {
	class World;

	class Listener {
	public:
		Listener(World* w);

		Listener(Listener&) = delete;
		Listener(Listener&& other);

		virtual ~Listener() = default;

		void update(const Matrix4& transform);

		virtual const Matrix4 &transformation() const = 0;

	private:
		Listener &operator=(Listener&) = default;

		size_t pos_in_world;
		World *world;
	};

}
