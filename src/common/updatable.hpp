#pragma once

#include "object.hpp"

#include <memory>
#include <vector>
#include <mutex>

class Updatable : virtual public Object
{
public:
	virtual ~Updatable() = default;

	virtual bool update(float dt, class UpdatableAdder& adder) = 0;
};

class UpdatableAdder
{
public:
	void add_updatable(std::shared_ptr<class Updatable> new_obj)
	{
		std::lock_guard l(mutex);
		new_elems.push_back(new_obj);
	}

protected:
	std::mutex mutex;
	std::vector<std::shared_ptr<class Updatable>> new_elems;
};
