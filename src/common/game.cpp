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

#include "game.hpp"

using namespace std;
using namespace Options;

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
frame()
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

	Audio::initialize();

	Renderer::initialize();

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

