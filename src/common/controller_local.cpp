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
}

void
ControllerLocal::update()
{
	int i;
	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	for(i = 0; i < players->size(); ++i)
		players->at(i)->update();
}

