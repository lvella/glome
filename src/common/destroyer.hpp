#pragma once

#include "ship.hpp"

class Destroyer: public Ship
{
public:
	Destroyer();
	virtual ~Destroyer();

	virtual void draw(Camera &c);
	virtual void update();

private:
	Fire other_jet;
};
