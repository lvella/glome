#include "controller_local.hpp"
#include "ai_controller.hpp"
#include "world_dummy.hpp"
#include "input.hpp"

using namespace std;

WorldDummy::WorldDummy()
{

	for(int i = 0; i < 2; i++)
	{
		ShipController* ctrl;
		AiController* ctrl_ai;
		Ship* s = new Ship(MeshTypes(rand() % (UFO + 1)));
		if(i > 0)
		{
			ctrl_ai = new AiController();
			s->set_controller(ctrl_ai);
			ai_controls.push_back(ctrl_ai);
			bot.push_back(s);
		}
		else
		{
			ctrl = Input::create_ship_controller(0);
			s->set_controller(ctrl);
			players.push_back(s);
		}
		ships.push_back(s);

	}
	_ctrl = new ControllerLocal(&ships, &bot, &ai_controls);
	_render = new Renderer(&ships);
	objects.insert(objects.begin(), ships.begin(), ships.end());
	objects.push_back(&cube);
	objects.push_back(&nova);
}

WorldDummy::~WorldDummy()
{
	delete _ctrl;
	delete _render;
}

void
WorldDummy::update()
{
	nova.update();
	_ctrl->update();
}

void
WorldDummy::draw()
{
	_render->draw(&objects);
}

