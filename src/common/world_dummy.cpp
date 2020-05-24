#include <chrono>

#include "controller_local.hpp"
#include "ai_controller.hpp"
#include "input.hpp"
#include "options.hpp"
#include "random.hpp"
#include "destroyer.hpp"
#include "ship_stats.hpp"
#include "projectile.hpp"
#include "thread_pool.hpp"
#include "time_accumulator.hpp"

#include "world_dummy.hpp"

using namespace std;

WorldDummy::WorldDummy()
{
	std::vector<Ship*> bot;
	std::vector<Ship*> players;
	ShipStats::shared_ptr stats(ShipStats::get());
	Ship* s;

	s = new Ship(Mesh::Type(Random::range(0, size_t(Mesh::Type::UFO))), stats);
	s->set_controller(Input::create_ship_controller(0));
	players.push_back(s);
	ships.push_back(s);

	for(int i = 0; i < Options::numBots; ++i)
	{
		AiController *ctrl_ai;

		s = new Ship(Mesh::Type(Random::range(0, size_t(Mesh::Type::UFO))), stats);
		ctrl_ai = new AiController();
		s->set_controller(ctrl_ai);
		ai_controls.push_back(ctrl_ai);
		bot.push_back(s);
		ships.push_back(s);
	}

	if(Options::showBotScreen && players.size() < 3) {
		players.insert(players.end(), bot.begin(), bot.begin() + min(bot.size(), 4 - players.size()));
	}
	_render = new Renderer(players, audio_world);

	_ctrl = new ControllerLocal(vector<Ship*>(ships), std::move(bot), vector<AiController*>(ai_controls));

	objects.push_back(&nova);
	objects.push_back(&cube);

	dynamic_objects.push_back(&nova);

	// Create flying spaghetti monsters
	const size_t NUM_FSMS = 5000;
	fsms.reserve(NUM_FSMS);
	for(size_t i = 0; i < NUM_FSMS; ++i) {
		fsms.emplace_back(audio_world);
	}

	dynamic_objects.reserve(dynamic_objects.size() + fsms.size());
	objects.reserve(objects.size() + fsms.size());
	for(auto &fsm : fsms) {
		dynamic_objects.push_back(&fsm);
		objects.push_back(&fsm);
	}

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
	static TimeAccumulator update_ta("Update objects");
	_ctrl->update();

	{
		constexpr size_t chunk_size = 400;
		TimeGuard timer(update_ta);

		parallel_run_and_wait(globalThreadPool, [&] (auto&& add_task) {
			for(size_t i = 0; i < dynamic_objects.size(); i += chunk_size) {
				add_task([&, i] {
					const size_t max = std::min(
						i + chunk_size,
						dynamic_objects.size()
					);

					for(size_t j = i; j < max; ++j) {
						dynamic_objects[j]->update();
					}
				});
			}
		});
	}

	{
		std::vector<VolSphere*> collision_objects;
		collision_objects.reserve(fsms.size());
		for(auto &fsm: fsms) {
			collision_objects.push_back(&fsm);
		}

		collision_tree.collide(
			Projectile::get_collision_volumes(),
			std::move(collision_objects)
		);
	}

	_render->audio_update();
}

void
WorldDummy::draw()
{
	_render->draw(objects);
}

