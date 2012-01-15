#include "controller_local.hpp"

#include "world_dummy.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	_ctrl = new ControllerLocal(&players, &bots);
	_render = new Renderer(&players);
	objects.insert(objects.begin(), players.begin(), players.end());
	objects.push_back(&cube);
	objects.push_back(&spg);
}

WorldDummy::~WorldDummy()
{
}

void
WorldDummy::update()
{
	_ctrl->update();
}

void
WorldDummy::draw()
{
	_render->draw(&objects);
}

