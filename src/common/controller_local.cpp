#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"

using namespace std;

ControllerLocal::ControllerLocal(vector<Ship*>* pp) : Controller(pp)
{
}

ControllerLocal::ControllerLocal(vector<Ship*>* pp, vector<ShipAI*>* pvb) : Controller(pp)
{
	bots = pvb;

	Ship* s = new Ship();
	players->push_back(s);
	ShipController* ctrl = Input::create_ship_controller(0);
	s->set_controller(ctrl);	
}

void
ControllerLocal::update()
{
	int i;
	for(i = 0; i < players->size(); ++i)
		players->at(i)->update();
}

