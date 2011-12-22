#pragma once

#include "object.hpp"

class Updatable : virtual public Object
{
public:
	virtual void update() = 0;
};

