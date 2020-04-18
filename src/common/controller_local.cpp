#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"
#include "projectile.hpp"

using namespace std;

void
ControllerLocal::update(float dt)
{
	int i;
	for(i = 0; i < ai_controllers.size(); ++i)
		ai_controllers[i]->act();

	for(i = 0; i < players.size(); ++i)
		players[i]->update(dt);
	/*for(i = 0; i < bots.size(); ++i)
		bots[i]->update();*/

	Projectile::update_all();
}

