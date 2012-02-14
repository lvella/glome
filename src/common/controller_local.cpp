#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"
#include "projectile.hpp"

using namespace std;

ControllerLocal::ControllerLocal(vector<Ship*>* pp, vector<Ship*>* bp, vector<AiController*>* ai) : Controller(pp)
{
	bots = bp;
	ai_controllers = ai;
}

ControllerLocal::~ControllerLocal()
{
	delete players->back();
	players->pop_back();
	if(bots){
		delete bots->back();
		bots->pop_back();
	}
}

void
ControllerLocal::update()
{
	int i;
	for(i = 0; i < ai_controllers->size(); ++i)
		ai_controllers->at(i)->act();

	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	for(i = 0; i < players->size(); ++i)
		players->at(i)->update();
	/*for(i = 0; i < bots->size(); ++i)
			bots->at(i)->update();*/

}

