#pragma once

#include <vector>

#include "ship.hpp"

class controller : public updatable
{
public:
	controller(std::vector<ship*>* pp);

protected:
	std::vector<ship*>* players;
};

