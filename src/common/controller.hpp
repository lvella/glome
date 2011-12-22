#pragma once

#include <vector>

#include "ship.hpp"

class Controller : public Updatable
{
public:
	virtual void update();
	Controller(std::vector<Ship*>* pp);

protected:
	std::vector<Ship*>* players;
};

