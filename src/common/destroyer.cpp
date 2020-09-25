#include <cstring>
#include "destroyer.hpp"

Destroyer::Destroyer():
	Ship(Mesh::Type::DESTROYER, ShipStats::get())
{
	float lala[] = {
			0.9999993443489075,
			1.1391634870960843e-06,
			-6.376084797921067e-07,
			-0.0011529988842085004,
			0.0,
			0.9999995231628418,
			5.463638785840885e-07,
			0.0009879996068775654,
			0.0,
			0.0,
			0.9999998211860657,
			-0.000552999961655587,
			0.0011529996991157532,
			-0.000987999141216278,
			0.0005529993213713169,
			0.9999986886978149,
	};

	Matrix4 lolo;
	memcpy(&lolo, &lala, sizeof(Matrix4));
	other_jet->set_t(QRot(lolo));
	other_jet->set_intensity(1.0f);
}

void Destroyer::create_sub_objects(std::vector<std::weak_ptr<SubObject>>& objs)
{
	Ship::create_sub_objects(objs);

	other_jet = std::make_shared<Fire>(weak_from_this(), 0.0006f);
	objs.push_back(other_jet);
}

void Destroyer::draw(Camera &c)
{
	c.setQRot(get_t());
	mesh->draw(c);
}

bool Destroyer::update(float dt, UpdatableAdder& adder)
{
	bool ret = Ship::update(dt, adder);
	other_jet->update(dt);

	return ret;
}
