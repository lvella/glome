#include "controller_local.hpp"
#include "ai_controller.hpp"
#include "input.hpp"
#include "options.hpp"
#include "random.hpp"

#include "world_dummy.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	std::vector<Ship*> bot;
	std::vector<Ship*> players;
	Ship* s;

	s = new Ship(MeshTypes(Random::range<0, UFO>()));
	s->set_controller(Input::create_ship_controller(0));
	players.push_back(s);
	ships.push_back(s);

	for(int i = 0; i < Options::numBots; ++i)
	{
		AiController *ctrl_ai;

		s = new Ship(MeshTypes(Random::range<0, UFO>()));
		ctrl_ai = new AiController();
		s->set_controller(ctrl_ai);
		ai_controls.push_back(ctrl_ai);
		bot.push_back(s);
		ships.push_back(s);
	}

	_ctrl = new ControllerLocal(&ships, &bot, &ai_controls);

	if(Options::showBotScreen && players.size() < 3) {
		players.insert(players.end(), bot.begin(), bot.begin() + min(bot.size(), 4 - players.size()));
	}
	_render = new Renderer(&players);

	objects.push_back(&nova);
	objects.push_back(&cube);
	objects.insert(objects.end(), ships.begin(), ships.end());
}

WorldDummy::~WorldDummy()
{
	delete _ctrl;
	delete _render;

	for(Ship* e : ships) {
		delete e;
	}
	ships.resize(0);

	for(AiController* e : ai_controls) {
		delete e;
	}
	ai_controls.resize(0);
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

