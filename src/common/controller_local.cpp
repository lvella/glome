#include <iostream>

#include "input.hpp"
#include "controller_local.hpp"
#include "projectile.hpp"

using namespace std;

void
ControllerLocal::update()
{
	int i;
	for(i = 0; i < ai_controllers.size(); ++i)
		ai_controllers[i]->act();

	//Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
	for(i = 0; i < players.size(); ++i)
		players[i]->update();
	/*for(i = 0; i < bots.size(); ++i)
		bots[i]->update();*/

	Projectile::update_all(Vector4(0.0f, 0.0f, 0.0f, -1.0f));
}

