#pragma once

#include <functional>

extern void initialize_registered();

class RegisterInitialization {
public:
	RegisterInitialization(std::function<void()>&& func);
};
