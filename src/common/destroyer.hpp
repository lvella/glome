#pragma once

#include "ship.hpp"

class Destroyer: public Ship
{
public:
	Destroyer();
	virtual ~Destroyer();

	void draw(Camera &c) override;
	void update() override;

private:
	Fire other_jet;
};
