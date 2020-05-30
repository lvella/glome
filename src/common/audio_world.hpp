#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "object.hpp"

namespace Audio {
	class Listener;
	class Source;

	class World {
		void addListener(Listener *l);

		void updateFromListener(float dt, size_t listener, const QRot& transform);

		std::vector<Listener*> listeners;

		friend class Source;
		friend class Listener;

	protected:
		void try_add_source(const std::shared_ptr<Object>& obj);
		std::vector<std::weak_ptr<Source>> sources;
	};

}
