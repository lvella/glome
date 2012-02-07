#include "controller_local.hpp"

#include "world_dummy.hpp"
#include "input.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	ShipController* ctrl = Input::create_ship_controller(0);
	Ship* s = new Ship(MeshTypes(rand() % MESH_COUNT));
	s->set_controller(ctrl);
	players.push_back(s);

	_ctrl = new ControllerLocal(&players);
	_render = new Renderer(&players);

	objects.insert(objects.begin(), players.begin(), players.end());
	objects.push_back(&cube);
	objects.push_back(&spg);
}

WorldDummy::~WorldDummy()
{
	delete _ctrl;
	delete _render;
}

void
WorldDummy::update()
{
	spg.update();
	_ctrl->update();
}

void
WorldDummy::draw()
{
	_render->draw(&objects);
}

