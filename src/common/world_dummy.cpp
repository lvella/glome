#include "controller_local.hpp"
#include "ai_controller.hpp"
#include "world_dummy.hpp"
#include "input.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	vector<Ship*> players;
	for(int i = 0; i < 1; i++)
	{
		ShipController* ctrl;
		Ship* s = new Ship(MeshTypes(rand() % (UFO + 1)));
		if(i > 0)
		{
			ctrl = new AiController();
			bot.push_back(s);
		}
		else
		{
			ctrl = Input::create_ship_controller(0);
			players.push_back(s);
		}
		s->set_controller(ctrl);
		ships.push_back(s);

		_ctrl = new ControllerLocal(&ships);
		_render = new Renderer(&ships);

	}
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

