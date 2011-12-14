#pragma once

#include "object.hpp"

class updatable : virtual public object
{
public:
	virtual void update() = 0;
};

