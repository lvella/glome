#include "controller_local.hpp"

using namespace std;

controller_local::controller_local(vector<ship*>* pp, vector<ship_ai*>* pb) : controller(pp)
{
	bots = pvb;

	ship* s = new ship();
	players.push_back(s);
	//ship_controller* ctrl = Input::create_ship_controller(0);
	//s->set_controller(ctrl);	
}

void
update()
{
	int i;
	for(i = 0; i < players.size(); ++i)
		players[i]->update();
}

