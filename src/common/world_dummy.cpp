#include "world_dummy.hpp"

#include <memory>
#include <stdio.h>

#include "input.hpp"
#include "options.hpp"
#include "random.hpp"
#include "supernova.hpp"
#include "spaghetti.hpp"
#include "thread_pool.hpp"

#include "openvr.h"


using namespace std;

WorldDummy::WorldDummy():
	meridians{std::make_shared<Meridians>()}
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

	// Loading the SteamVR Runtime
	vr::EVRInitError eError = vr::VRInitError_None;
	// m_pHMD = std::make_shared<vr::IVRSystem>(
	// 	vr::VR_Init( &eError, vr::VRApplication_Scene )
	// );
	m_pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );

	if ( eError != vr::VRInitError_None )
	{
		m_pHMD = nullptr;
		printf("Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		printf("Launching in non-VR mode\n");
		_render = new Renderer(std::move(players), *this);
	}
	else if (players.size() > 1)
	{
		std::cout << "VR only supported on single player" << std::endl;
		_render = new Renderer(std::move(players), *this);
	}
	else
	{
		std::cout << "Launching glome in VR mode" << std::endl;
		_render = new RendererVR(std::move(players), *this, m_pHMD );
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

WorldDummy::~WorldDummy()
{
	delete _render;

	for(AiController* e : ai_controls) {
		delete e;
	}
	ai_controls.resize(0);

	vr::VR_Shutdown();
	m_pHMD = NULL;
}
