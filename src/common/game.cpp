#include "game.hpp"

//#include "menu.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "particle_system.hpp"
#include "world_dummy.hpp"
#include "random.hpp"
#include "dustfield.hpp"
#include "audio.hpp"

namespace Game
{
RunContext* context;


class Paused: public RunContext {
	void setup_display() {
	}

	void draw() {
	}

	void update() {
	}
};

static std::unique_ptr<World> world;
static std::unique_ptr<Paused> paused;

void
frame(std::chrono::duration<float> dt)
{
	context->update();
	context->draw();
}

void
initialize()
{
	// OpenGL nonchanging settings
	glEnableVertexAttribArray(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);

	// Must be the first to initialize, so shaders
	// can be created with the correct perspective
	// matrix.
	CamShader::initialize(float(Options::width) / float(Options::height));
	Renderer::initialize();

	Audio::initialize();

	world.reset(new WorldDummy());
	paused.reset(new Paused());

	MiniMap::initialize();
	Projectile::initialize();
	ParticleSystem::initialize();
	DustField::initialize();
	//Menu::initialize();
	initialize_meridians();

	switch_state(WORLD);
}

void
shutdown()
{
	Audio::shutdown();
}

void switch_state(state s)
{
	switch(s)
	{
	case EXIT:
		//running = true;
		break;
	case MENU:
		context = paused.get();
		break;
	case WORLD:
		context = world.get();
		break;
	}
	context->setup_display();
}
}

