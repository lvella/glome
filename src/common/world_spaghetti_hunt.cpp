#include "world_spaghetti_hunt.hpp"

#include <memory>
#include <stdio.h>
#include <openvr.h>

#include "input.hpp"
#include "options.hpp"
#include "random.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"
#include "thread_pool.hpp"
#include "fatal_error.hpp"

using namespace std;

WorldSpaghettiHunt::WorldSpaghettiHunt(vr::IVRSystem* hmd):
	meridians{std::make_shared<Meridians>()},
	hmd{hmd}
{
	ShipStats::shared_ptr stats(ShipStats::get());

	// Create single player:
	{
		auto s = std::make_shared<Ship>(
			Mesh::Types(Random::range(0, Mesh::UFO)), stats
		);

		controller = std::make_shared<ShipController>();
		Input::register_ship_controller(0, controller);
		s->set_controller(controller);

		player = s;
		ships.push_back(s);

		add_updatable(std::move(s));
	}

	if(hmd) {
		_render = std::make_unique<RendererVR>(player, *this, hmd);
	} else {
		_render = std::make_unique<FullViewRenderer<ScoreRenderer>>(
			player, *this
		);
	}

	// Add unmanaged meridians
	add_unmanaged(meridians);

	// Create supernova
	add_updatable(std::make_shared<Supernova>());

	// Create flying spaghetti monsters in parallel
	constexpr size_t NUM_FSMS = 5000;
	Adder adder;
	globalThreadPool.parallel_run_and_wait([&] (auto&& add_task) {
		for(size_t i = 0; i < NUM_FSMS; i += CHUNCK_SIZE) {
			add_task([&, i] {
				const size_t max = std::min(
					i + CHUNCK_SIZE,
					NUM_FSMS
				);

				for(size_t j = i; j < max; ++j) {
					adder.add(
						std::make_shared<Spaghetti>()
					);
				}
			});
		}
	});
	adder.add_elems_to_world(*this);

	std::cout << "World initialized!" << std::endl;
}

bool WorldSpaghettiHunt::is_alive()
{
	if(was_alive && player.expired()) {
		if(!hmd) {
			ScoreRenderer &sr = static_cast<ScoreRenderer&>(*_render);
			auto new_render = std::make_unique<FullViewRenderer<
				ScoreGameOverRenderer>>(
					controller,
					std::move(sr)
				);

			game_over_render = new_render.get();
			_render = std::move(new_render);
		}

		was_alive = false;
	}

	return !(game_over_render && game_over_render->done());
}
