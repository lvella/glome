#pragma once

#include <vector>

#include "ship.hpp"

class Controller : public Updatable
{
public:
	Controller() {}
	virtual ~Controller() {};

	virtual void update() = 0;
	Controller(std::vector<Ship*>* pp);

protected:
	std::vector<Ship*>* players;
};

