#include <algorithm>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "4dmath.hpp"
#include "projectile.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "shader.hpp"
#include "../sdl/input.hpp"
#include "../sdl/kbinput.hpp"
#include "../sdl/jsinput.hpp"
#include "../sdl/net_input.hpp"
#include "protocol.hpp"
#include "options.hpp"
#include "udp_server.hpp"
#include "mesh.hpp"

#include "net_world.hpp"

static GLuint program;
extern boost::asio::io_service gIOService;

using namespace std;
using namespace boost::asio::ip;
using namespace Options;

void NetWorld::initialize()
{
#include "world_proj.glsl.hpp"
  program = setup_vshader(world_proj_glsl, world_proj_glsl_len);
}

Ship*
NetWorld::next_ship(const Matrix4& ref)
{
  ships.push_back(new Ship(DESTROYER));
  interpols.push_back(Interpol());
  ships[ships.size() - 1]->setTransformation(ref);
  return ships[ships.size() - 1];
}

void
NetWorld::handle_socket(const boost::system::error_code& error, std::size_t bytes)
{
  if(!error)
  {
	NetInput::parse_message(recv_buf, bytes, true);
  }

  cl_socket->async_receive_from(boost::asio::buffer(recv_buf),
                          *receiver_endpoint,
                          boost::bind(&NetWorld::handle_socket, this, boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

NetWorld::NetWorld(bool isc, string host, short int port):
		interp(false),
		param_t(0.0f),
		cam_pos(Vector4(0.0f, 0.0f, 0.0f, -1.0f))
{
  ships.push_back(new Ship(HUNTER));
  interpols.push_back(Interpol());
  Input::Kb::set_ship(ships[0]);

  cam_hist.resize(10, Matrix4::IDENTITY);

  isClient = isc;
  isServer = !isc;
  if(isc)
  {
    receiver_endpoint = new boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(host), port);

    cl_socket = new udp::socket(gIOService);
    cl_socket->open(udp::v4());

    boost::array<int, 1> v = {JOIN_GAME};
    cl_socket->send_to(boost::asio::buffer(v), *receiver_endpoint);
    boost::thread service(boost::bind(&boost::asio::io_service::run, &gIOService));

    cl_socket->async_receive_from(boost::asio::buffer(recv_buf),
                        *receiver_endpoint,
                        boost::bind(&NetWorld::handle_socket, this, boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));

  }
}

void
NetWorld::setup_display()
{
	World::setup_display();

	// Since the projection is not changing, it can be here.
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, double(width) / double(height), 0.001, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void
NetWorld::handle_send(const boost::system::error_code& error, std::size_t bytes)
{
  if(error)
  {
    cout << "Error: Could not send." << endl;
  }
  else
  {
    cout << "Sent " << bytes << " bytes." << endl;
  }
}

void
NetWorld::set_interpolation(unsigned int s_id, const Matrix4& f, const Matrix4& t)
{
  interpols[s_id].interp = true;
  interpols[s_id].from = f;
  interpols[s_id].to = t;
  interpols[s_id].param_t = 0.0f;
}


bool
NetWorld::update()
{
  bool run;
  // Treat events
  run = Input::handle();

  Projectile::update_all(cam_pos);

  Vector4 c = cube.transformation().position();

  for(int e = 0; e < ships.size(); ++e)
  {
	Interpol& i = interpols[e];
    if(!i.interp)
      ships[e]->update();
    else
    {
	  if(i.param_t > 1.1f)
	  {
	    i.interp = false;
	    i.param_t = 0.f;
	  }
	  else
	  {
	    ships[e]->setTransformation(i.from.interpolation(i.to, i.param_t));
	    //cout << "Interpolando: " << param_t * 10 << '\n' << ships[0]->transformation() << endl;
	    i.param_t += 0.001f;
	  }
    }
  }

  spg.update();

/*
  for(int i = 0; i < ships.size(); ++i)
  {
    ships[i]->update();
    if(Projectile::collide(ships[i]))
      ships[i]->setTransformation(cube.transformation() * yw_matrix(M_PI));

    if((c - ships[i]->transformation().position()).squared_length() < (0.03f * 0.03f))
    {
      cube.randomize();
      //std::cout << "Ship " << i << " scored " << ++points[i] << " points!" << std::endl;
    }
  }
*/

  // Network update
  Ship* ship = ships[0];
  const vector<float>& v = ship->getMessage();
  if(v.size() > 0)
  {
	if(isClient)
      int re = cl_socket->send_to(boost::asio::buffer(v), *receiver_endpoint);
	else
	  Server::send_to_all(v, v.size() * sizeof(float), 0, true);

    ship->clearMessage();
  }

  return run;
}

void
NetWorld::draw()
{
  const Matrix4 offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Matrix4 center = ships[0]->transformation().transpose();

  // Camera transform
  Matrix4 camera = offset * cam_hist.front();
  camera.loadToGL();
  cam_pos = Vector4(-camera[3][0], -camera[3][1], -camera[3][2], -camera[3][3]);
  cam_hist.pop_front();
  cam_hist.push_back(center);

  glUseProgram(program);
  draw_meridians();

  cube.draw();
  spg.draw();
  Projectile::draw_all();
  glUseProgram(program);
  for(int i = 0; i < ships.size(); ++i)
    ships[i]->draw();

  MiniMap::draw(0, this, center);
}

void
NetWorld::fill_minimap()
{
	MiniMap::draw_dot(cube);
	for(int i = 1; i < ships.size(); ++i)
		MiniMap::draw_dot(*ships[i]);
}
