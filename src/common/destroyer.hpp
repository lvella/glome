#pragma once

#include "ship.hpp"

class Destroyer final: public Ship
{
public:
	Destroyer();

	void draw(Camera &c) override;
	bool update(float dt, UpdatableAdder& adder) override;

private:
	Fire other_jet;
};
