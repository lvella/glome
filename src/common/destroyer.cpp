#include <cstring>
#include "destroyer.hpp"

Destroyer::Destroyer():
	Ship(Mesh::DESTROYER),
	other_jet(0.0006f)
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
	other_jet.set_transformation(lolo);
	other_jet.setIntensity(1.0f);
}

Destroyer::~Destroyer()
{
}

void Destroyer::draw(Camera &c)
{
	c.pushMultMat(_t);
	mesh->draw(c);
	fx_engine.draw(c);
	other_jet.draw(c);
	c.popMat();
}

void Destroyer::update()
{
	Ship::update();
	other_jet.update();
}
