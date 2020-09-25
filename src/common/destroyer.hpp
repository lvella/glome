#pragma once

#include "ship.hpp"
#include <memory>

class Destroyer final: public Ship
{
public:
	Destroyer();

	void create_sub_objects(std::vector<std::weak_ptr<SubObject>>&) override;

	void draw(Camera &c) override;
	bool update(float dt, UpdatableAdder& adder) override;

private:
	std::shared_ptr<Fire> other_jet;
};
