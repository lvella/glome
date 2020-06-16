#include "initialization.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace {
	bool initialized = false;
	std::unique_ptr<std::vector<std::function<void()>>> ini_list;
}

RegisterInitialization::RegisterInitialization(std::function<void()>&& func)
{
	assert(!initialized);
	if(!ini_list) {
		ini_list = std::make_unique<std::vector<std::function<void()>>>();
	}

	ini_list->emplace_back(std::move(func));
}

void initialize_registered()
{
	assert(!initialized);
	initialized = true;

	if(!ini_list) {
		return;
	}
	auto list = std::move(ini_list);

	for(auto& f: *list) {
		f();
	}
}
