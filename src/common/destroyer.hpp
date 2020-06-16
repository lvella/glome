#pragma once

#include "ship.hpp"
#include <memory>

class Destroyer final: public Ship
{
public:
	Destroyer();

	std::vector<std::weak_ptr<SubObject>> create_sub_objects() override;

	void draw(Camera &c) override;
	bool update(float dt, UpdatableAdder& adder) override;

private:
	std::shared_ptr<Fire> other_jet;
};
