#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "4dmath.hpp"
#include "matrix4.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "init_gl.hpp"
#include "udp_server.hpp"
#include "world.hpp"

using namespace std;
using namespace boost::asio::ip;

extern const int HEIGHT = 600;
extern const int WIDTH = 800;

extern const float FOV = 45.0f;

boost::asio::io_service gIOService;
udp_server* server;
udp::socket* cl_socket;
udp::endpoint* receiver_endpoint;
bool isServer;
bool isClient;
string orig;
string host;
short port;

/* void update()
{
  Drawable::update_all();
  Projectile::update_all();
  ship.update();

  if(isServer)
  {
    //ship.update();
    server->update();
  }
  else if(isClient)
  {
        const vector<int>& v = ship.getMessage();
    if(v.size() > 0)
    {
      int re = cl_socket->send_to(boost::asio::buffer(v), *receiver_endpoint);
      //cout << "Size: " << v.size() << " | RE: " << re << endl;
      ship.clearMessage();

      //cout << "Old matrix: " << endl;
      //cout << ship.transformation() << endl;

      boost::array<float, 16> recv_buf;
      udp::endpoint sender_endpoint;
      size_t len = cl_socket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
      if(len < (sizeof(float) * 16))
        cout << "Error" << endl;
      else
      {
        //cout << "Setando" << endl;
        Matrix4 m;
        int k;
        for(int i = 0, k = 0; i < 4; ++i)
          for(int j = 0; j < 4; ++j, ++k)
            m[i][j] = recv_buf[k];
        ship.set(m);
      }

      //cout << "New matrix: " << endl;
      //cout << ship.transformation() << endl;
    }
  }
}*/

void main_loop()
{
  static World world;

  const int FPS = 60;
  uint64_t frame_count = 0;
  bool running = true;
  Uint32 ticks = SDL_GetTicks();

  while(running) {
    running = world.update();

    world.draw();
    SDL_GL_SwapBuffers();

    // Fix framerate
    {
      // TODO: maybe clk_div is useful here...
      const int period = 1000/60;
      Uint32 now = SDL_GetTicks();
      int delay = period - int(now - ticks);
      if(delay > 0)
	      SDL_Delay(delay);
      ticks = now;
    }
    ++frame_count;
  }

  cout << frame_count << " frames rendered." << endl;
}

int main(int argc, char **argv)
{
  srand(time(NULL));

  // SDL startup
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
    return 1;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL);
  SDL_WM_SetCaption("Navigna", NULL);
  //SDL_ShowCursor(SDL_DISABLE);

  init_gl();

  // OpenGL nonchanging settings
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glLineWidth(1.5f);
  glAlphaFunc(GL_NOTEQUAL, 0.0f);

  // Fancy OpenGL fog
  {
    const float fcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glFogfv(GL_FOG_COLOR, fcolor);
  }
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogf(GL_FOG_DENSITY, 0.9f);
  glHint(GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_END, 3.0f);
  glFogf(GL_FOG_START, 2.0f);
  glEnable(GL_FOG);

  // 3D to 2D projection
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV, double(WIDTH) / double(HEIGHT), 0.001, 5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 4D to 3D projection
  initialize_meridians();
  World::initialize();
  MiniMap::initialize();
  Ship::initialize();
  Projectile::initialize();

  // Configure network
  if(argc == 1)
  {
    isServer = isClient = false;
  }
  else
  {
    if(argc == 4)
    {
      host = argv[2];
      port = atoi(argv[3]);
    }
    else if(argc == 5)
    {
      orig = argv[2];
      host = argv[3];
      port = atoi(argv[4]);
    }
    isServer = (atoi(argv[1]) == 0);
    isClient = !isServer;
  }
  if(isServer)
  {
    server = new udp_server(port);
  }
  else if(isClient)
  {
    receiver_endpoint = new udp::endpoint(udp::v4(), port);
    receiver_endpoint->address(boost::asio::ip::address::from_string(host));

    cl_socket = new udp::socket(gIOService);
    cl_socket->open(udp::v4());
  }

  main_loop();

  SDL_Quit();
}

