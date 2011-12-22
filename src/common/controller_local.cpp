#include "controller_local.hpp"

using namespace std;

ControllerLocal::ControllerLocal(vector<Ship*>* pp, vector<Ship_ai*>* pb) : Controller(pp)
{
	bots = pvb;

	Ship* s = new Ship();
	players->push_back(s);
	//ship_controller* ctrl = Input::create_ship_controller(0);
	//s->set_controller(ctrl);	
}

void
update()
{
	int i;
	for(i = 0; i < players.size(); ++i)
		players->at(i)->update();
}

