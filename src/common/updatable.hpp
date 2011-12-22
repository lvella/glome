#pragma once

#include "object.hpp"

class Updatable : virtual public Object
{
public:
	Updatable() {};
	virtual ~Updatable() {};

	virtual void update() = 0;
};

