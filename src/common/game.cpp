#include "game.hpp"

#include "gltext.hpp"
#include "profiling.hpp"
#include "audio.hpp"
#include "game_state_machine.hpp"
#include <memory>

namespace
{

RunContext* context;

std::unique_ptr<GameStateMachine> game_state_machine;

void advance_game_state()
{
	assert(game_state_machine);
	context = game_state_machine->get_next_context();

	assert(context);
	context->setup_display();
}

} // anonymous namespace


namespace Game
{

void
frame(std::chrono::duration<float> frame_time)
{
	globalProfiler.maybe_print();
	static TimeAccumulator& frame_ta = globalProfiler.newTimer("Full frame");

	TimeGuard timer(frame_ta);
	constexpr float max_physics_dt = 1.0 / MIN_FPS;
	const float dt = std::min(max_physics_dt, frame_time.count());

	while(!context->update(dt)) {
		advance_game_state();
	}

	context->draw();
}

void
initialize()
{
	bool glt_inited = gltInit();
	assert(glt_inited);

	Renderer::initialize();

	// Must be the first to initialize, so shaders
	// can be created with the correct perspective
	// matrix.
	CamShader::initialize(float(Options::width) / float(Options::height));

	initialize_registered();

	game_state_machine = std::make_unique<DummyStateMachine>();
	advance_game_state();
}

void
shutdown()
{
	Audio::shutdown();
	gltTerminate();
}

} // namespace Game
