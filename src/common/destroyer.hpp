#pragma once

#include "ship.hpp"
#include <memory>

class Destroyer final: public Ship
{
public:
	Destroyer(ShipStats::shared_ptr& sstats);

	void create_sub_objects(std::vector<std::weak_ptr<SubObject>>&) override;

	bool update(float dt, UpdatableAdder& adder) override;

private:
	std::shared_ptr<Fire> other_jet;
};
