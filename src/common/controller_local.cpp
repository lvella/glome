#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"
#include "projectile.hpp"

using namespace std;

ControllerLocal::ControllerLocal(vector<Ship*>* pp) : Controller(pp)
{
	Ship* s = new Ship();
	players->push_back(s);
	ShipController* ctrl = Input::create_ship_controller(0);
	s->set_controller(ctrl);

	/*bots = pvb;
	if(bots) {
		ShipAI* enemy = new ShipAI();
		bots->push_back(enemy);
	}*/
}

ControllerLocal::~ControllerLocal()
{
	delete players->back();
	players->pop_back();
	/*if(bots){
		delete bots->back();
		bots->pop_back();
	}*/
}

void
ControllerLocal::update()
{
	int i;
	/*for(i = 0; i < bots->size(); ++i)
		bots->at(i)->act();*/

	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	for(i = 0; i < players->size(); ++i)
		players->at(i)->update();
	/*for(i = 0; i < bots->size(); ++i)
			bots->at(i)->update();*/

}

