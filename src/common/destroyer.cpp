#include <cstring>
#include "destroyer.hpp"

Destroyer::Destroyer(ShipStats::shared_ptr& sstats):
	Ship(Mesh::Type::DESTROYER, sstats, 0.0012)
{}

void Destroyer::create_sub_objects(std::vector<std::weak_ptr<SubObject>>& objs)
{
	Ship::create_sub_objects(objs);

	other_jet = std::make_shared<Fire>(weak_from_this(), 0.0012f);

	fx_engine->set_t(yw_qrot(0.0007) * xw_qrot(-0.0012));
	other_jet->set_t(yw_qrot(0.0007) * xw_qrot(0.0012));

	objs.push_back(other_jet);
}

bool Destroyer::update(float dt, UpdatableAdder& adder)
{
	bool ret = Ship::update(dt, adder);
	other_jet->set_intensity(fx_engine->get_intensity());
	other_jet->update(dt);

	return ret;
}
