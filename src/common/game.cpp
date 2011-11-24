//#include "../sdl/menu.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "split_world.hpp"
#include "net_world.hpp"
#include "udp_server.hpp"
#include "minimap.hpp"
#include "projectile.hpp"

#include "game.hpp"

using namespace std;
using namespace Options;

/* TODO: Remove this shit from here. */
boost::asio::io_service gIOService;

namespace Game
{
	World* world;

	void frame()
	{
		world->update();
		world->draw();
	}

	void initialize()
	{
		// OpenGL nonchanging settings
	  glEnable(GL_TEXTURE_2D);
	  glEnableVertexAttribArray(0);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);

		World::initialize();

		if(isSplit)
		{
			world = new SplitWorld();
		}
		else
		{
			if(isServer)
				Server::initialize(port);
			world = new NetWorld(isClient, host, port);
		}
		world->setup_display();

		MiniMap::initialize();
		Projectile::initialize();
		//Menu::initialize();
		initialize_meridians();
	}

	void switch_state(state s)
	{
		switch(s) {
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
