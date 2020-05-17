#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "qrot.hpp"

namespace Audio {
	class Listener;
	class Source;

	class World {
		void addListener(Listener *l);

		void updateFromListener(size_t listener, const QRot& transform);

		std::vector<std::weak_ptr<Source>> sources;
		std::vector<Listener*> listeners;

		friend class Source;
		friend class Listener;
	};

}
