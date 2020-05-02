#pragma once

#include <unordered_set>
#include <vector>
#include "audio_source.hpp"
#include "audio_listener.hpp"

namespace Audio {
	class Listener;
	class Source;

	class World {
		void addListener(Listener *l);

		void updateFromListener(size_t listener, const QRot& transform);

		std::unordered_set<Source*> sources;
		std::vector<Listener*> listeners;

		friend class Source;
		friend class Listener;
	};

}
