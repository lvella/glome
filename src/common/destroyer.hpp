#pragma once

#include "ship.hpp"

class Destroyer final: public Ship
{
public:
	Destroyer();

	void draw(Camera &c) override;
	void update(float dt) override;

private:
	Fire other_jet;
};
