#include "options.hpp"
#include "meridian.hpp"
#include "menu.hpp"
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

	static bool running = true;

	bool frame()
	{
		world->update();
		world->draw();
		return running;
	}

	void initialize()
	{
		// OpenGL nonchanging settings
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glEnableClientState(GL_VERTEX_ARRAY);

		if(isSplit)
		{
			SplitWorld::initialize();
			world = new SplitWorld();
		}
		else
		{
			NetWorld::initialize();
			if(isServer)
				Server::initialize(port);
			world = new NetWorld(isClient, host, port);
		}
		world->setup_display();

		MiniMap::initialize();
		Projectile::initialize();
		Menu::initialize();
		initialize_meridians();
	}

	void switch_state(state s)
	{
		switch(s) {
		case EXIT:
			running = true;
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
