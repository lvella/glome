#pragma once

#include <memory>
#include <vector>
#include <mutex>

#include "object.hpp"
#include "object_adder.hpp"

using UpdatableAdder = ObjectAdder<std::shared_ptr<class Updatable>>;

class Updatable : virtual public Object
{
public:
	virtual ~Updatable() = default;

	virtual bool update(float dt, UpdatableAdder& adder) = 0;
};
