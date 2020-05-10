#pragma once

#include "object.hpp"

#include <memory>

class UpdatableAdder
{
public:
	virtual ~UpdatableAdder() = default;

	virtual void add_updatable(std::shared_ptr<class Updatable> new_obj) = 0;
};

class Updatable : virtual public Object
{
public:
	virtual ~Updatable() = default;

	virtual bool update(float dt, UpdatableAdder& adder) = 0;
};

