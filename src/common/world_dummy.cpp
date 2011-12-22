#include "world_dummy.hpp"

#include "controller_local.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	_render = new Render(&players);
	_ctrl = new ControllerLocal(&players, &bots);
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
	_render->draw();
}

