#pragma once

#include "object.hpp"

class Updatable : virtual public Object
{
public:
	virtual ~Updatable() = default;

	virtual void update(float dt) = 0;
};

