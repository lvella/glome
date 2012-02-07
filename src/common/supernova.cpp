#include "supernova.hpp"

Supernova::Supernova():
	t(0.0f),
	mesh(ICOSPHERE)
{}

Supernova::~Supernova()
{
	Mesh::release_mesh(mesh);
}

void Supernova::update()
{
}
