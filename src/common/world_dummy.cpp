#include "world_dummy.hpp"

#include <memory>

#include "input.hpp"
#include "options.hpp"
#include "random.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	std::vector<std::weak_ptr<Ship>> bot;
	std::vector<std::weak_ptr<Ship>> players;
	ShipStats::shared_ptr stats(ShipStats::get());

	// Create single player:
	{
		auto s = std::make_shared<Ship>(
			Mesh::Types(Random::range(0, Mesh::UFO)), stats
		);
		s->set_controller(Input::create_ship_controller(0));
		players.push_back(s);
		ships.push_back(s);

		add_updatable(std::move(s));
	}

	for(int i = 0; i < Options::numBots; ++i)
	{
		AiController *ctrl_ai;

		auto s = std::make_shared<Ship>(
			Mesh::Types(Random::range(0, Mesh::UFO)), stats
		);
		ctrl_ai = new AiController();
		s->set_controller(ctrl_ai);
		ai_controls.push_back(ctrl_ai);
		bot.push_back(s);
		ships.push_back(s);

		add_updatable(std::move(s));
	}

	if(Options::showBotScreen && players.size() < 3) {
		players.insert(players.end(), bot.begin(),
			bot.begin() + min(bot.size(), 4 - players.size()));
	}
	_render = new Renderer(std::move(players), *this);

	// Create supernova
	add_updatable(std::make_shared<Supernova>());

	// Create flying spaghetti monsters
	const size_t NUM_FSMS = 5000;
	for(size_t i = 0; i < NUM_FSMS; ++i) {
		add_updatable(std::make_shared<Spaghetti>(*this));
	}
}

WorldDummy::~WorldDummy()
{
	delete _render;

	for(AiController* e : ai_controls) {
		delete e;
	}
	ai_controls.resize(0);
}
