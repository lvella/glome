//#include "menu.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "particle_system.hpp"
#include "world_dummy.hpp"

#include "game.hpp"

using namespace std;
using namespace Options;

namespace Game
{
World* world;

void
frame()
{
	world->update();
	world->draw();
}

void
initialize()
{
	// OpenGL nonchanging settings
	glEnable(GL_TEXTURE_2D);
	glEnableVertexAttribArray(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);

	Renderer::initialize();

	world = new WorldDummy();
	world->setup_display();

	MiniMap::initialize();
	Projectile::initialize();
	ParticleSystem::initialize();
	//Menu::initialize();
	initialize_meridians();
}

void switch_state(state s)
{
	switch(s)
	{
	case EXIT:
		//running = true;
		break;
	case MENU:
		// TODO...
		break;
	case WORLD:
		// TODO...
		break;
	}
}
}

