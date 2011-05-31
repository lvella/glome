#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <deque>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "4dmath.hpp"
#include "input.hpp"
#include "matrix4.hpp"
#include "minimap.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "randomcube.hpp"
#include "shader.hpp"
#include "drawable.hpp"
#include "init_gl.hpp"
#include "udp_server.hpp"

using namespace std;
using namespace boost::asio::ip;

extern const int HEIGHT = 600;
extern const int WIDTH = 800;

extern const float FOV = 45.0f;

double s[360];
double c[360];
double z[360];

// Camera transform historic
std::deque<Matrix4> cam_hist(10, Matrix4::IDENTITY);

Ship ship;
//Ship* ship2;
Input input(&ship);
RandomCube* cube;
boost::asio::io_service gIOService;
udp_server* server;
udp::socket* cl_socket;
udp::endpoint* receiver_endpoint;
bool isServer;
bool isClient;
string orig;
string host;
short port;

static GLuint program;

void initialize_vars()
{
  for(int i = 0; i < 360; ++i)
  {
    double n = i * M_PI / 180;
    c[i] = cos(n);
    s[i] = sin(n);
    z[i] = 0.0;
  }
}

void initialize_shader()
{
#include "world_proj.glsl.hpp"
  program = setup_vshader(world_proj_glsl, world_proj_glsl_len);
}

void draw_meridian(const double *a, const double *b, const double *c, const double *d)
{
  glBegin(GL_LINES);
  for(int i = 0; i < 360; ++i)
  {
    glVertex4d(a[i], b[i], c[i], d[i]);
  }
  glEnd();
}

void draw()
{
  const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));

  // Camera transform
  (offset * cam_hist.front()).loadToGL();
  cam_hist.pop_front();
  cam_hist.push_back(ship.transformation().transpose());

  glUseProgram(program);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(.0f, .0f, 1.0f);
  draw_meridian(z, z, s, c);
  glColor3f(.0f, 1.f, .0f);
  draw_meridian(z, s, z, c);
  glColor3f(1.0f, .0f, .0f);
  draw_meridian(s, z, z, c);
  glColor3f(1.0f, 1.0f, .0f);
  draw_meridian(s, c, z, z);
  glColor3f(1.0f, .0f, 1.0f);
  draw_meridian(s, z, c, z);
  glColor3f(.0f, 1.0f, 1.0f);
  draw_meridian(z, s, c, z);

  //cube.draw();
  Drawable::draw_all();
  Projectile::draw_all();
  glUseProgram(program);
  ship.draw();
  MiniMap::draw();

  SDL_GL_SwapBuffers();
}

void update()
{
  Drawable::update_all();
  Projectile::update_all();
  ship.update();

  if(isServer)
  {
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
    }
  }
}

void main_loop()
{
  const int FPS = 60;
  uint64_t frame_count = 0;
  bool running = true;
  Uint32 ticks = SDL_GetTicks();

  while(running) {
    // Treat events
    {
      input.handle(running);
    }

    update();
    draw();

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
  glFogi(GL_FOG_MODE, GL_EXP);
  {
    const float fcolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glFogfv(GL_FOG_COLOR, fcolor);
  }
  glFogf(GL_FOG_DENSITY, 0.55f);
  glHint(GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_END, M_PI);
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
  initialize_shader();
  initialize_vars();
  MiniMap::initialize();
  ship.initialize();
  Projectile::initialize();
  cube = (RandomCube*)Drawable::create_random_cube();
  //ship2 = (Ship*)Drawable::create_ship();

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

