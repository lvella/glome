#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"
#include "projectile.hpp"

using namespace std;

ControllerLocal::ControllerLocal(vector<Ship*>* pp) : Controller(pp)
{
}

ControllerLocal::ControllerLocal(vector<Ship*>* pp, vector<ShipAI*>* pvb) : Controller(pp)
{
	bots = pvb;

	Ship* s = new Ship(MeshTypes(rand() % MESH_COUNT));
	players->push_back(s);
	ShipController* ctrl = Input::create_ship_controller(0);
	s->set_controller(ctrl);	
}

void
ControllerLocal::update()
{
	int i;
	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	for(i = 0; i < players->size(); ++i)
		players->at(i)->update();
}

