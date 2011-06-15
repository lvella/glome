#include <algorithm>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "4dmath.hpp"
#include "projectile.hpp"
#include "minimap.hpp"
#include "meridian.hpp"
#include "shader.hpp"
#include "input.hpp"
#include "kbinput.hpp"
#include "jsinput.hpp"
#include "protocol.hpp"
#include "main.hpp"

#include "net_world.hpp"

static GLuint program;
extern boost::asio::io_service gIOService;

using namespace std;
using namespace boost::asio::ip;

void NetWorld::initialize()
{
#include "world_proj.glsl.hpp"
  program = setup_vshader(world_proj_glsl, world_proj_glsl_len);
}

Ship*
NetWorld::next_ship(const Matrix4& ref)
{
  ships.push_back(new Ship());
  ships[ships.size() - 1]->setTransformation(ref);
  return ships[ships.size() - 1];
}

void
NetWorld::handle_socket(const boost::system::error_code& error, std::size_t bytes)
{
  if(!error)
  {
    unsigned int nums = bytes / sizeof(int);
    unsigned int i = 0;
    int s_id;
    int x, y;
    Matrix4 t;
    boost::array<float, 16>::iterator it;
    while(i < nums)
    {
      switch(int(recv_buf[i]))
      {
      case INIT_POS:
        it = recv_buf.begin() + i + 1;
        copy(it, it + 16, &t[0][0]);
        ships[0]->setTransformation(t);
        i += 17;
        break;
      case NEW_SHIP:
        s_id = recv_buf[++i];
        //if(s_id != ships.size())
          //cout << "It should not happen.." << endl;
        it = recv_buf.begin() + i + 1;
        copy(it, it + 16, &t[0][0]);
        i += 17;
        next_ship(t);
        break;
      case UPDATE_SHIP:
        s_id = recv_buf[++i];
        //cout << "UPDATE SHIP " << s_id << endl;
        it = recv_buf.begin() + i + 1;
        copy(it, it + 16, &t[0][0]);
        i += 17;
        if(s_id >= ships.size())
          cout << "Trying to update position of ship " << s_id << ", but i have only " << ships.size() << '.' << endl;

        if(s_id == 0)
        {
          interp = true;
          param_t = 0.0f;
          interp_from = ships[0]->transformation();
          interp_to = t;
          //cout << "Setando interpolação:\nFrom:\n" << interp_from << "To:\n" << interp_to << endl;
        }
        else
          ships[s_id]->setTransformation(t);
        break;
      }
    }
  }

  cl_socket->async_receive_from(boost::asio::buffer(recv_buf),
                          *receiver_endpoint,
                          boost::bind(&NetWorld::handle_socket, this, boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

NetWorld::NetWorld(bool isc, string host, short int port):
		interp(false),
		param_t(0.0f)
{
  ships.push_back(new Ship());
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

  // 3-D to 2-D projection
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV, double(WIDTH) / double(HEIGHT), 0.001, 5);
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

bool
NetWorld::update()
{
  bool run;
  // Treat events
  run = Input::handle();

  Projectile::update_all();

  Vector4 c = cube.transformation().position();

  if(!interp)
    ships[0]->update();
  else
  {
	if(param_t > 1.1f)
	{
	  interp = false;
	  param_t = 0.f;
	}
	else
	{
	  ships[0]->setTransformation(interp_from.interpolation(interp_to, param_t));
	  //cout << "Interpolando: " << param_t * 10 << '\n' << ships[0]->transformation() << endl;
	  param_t += 0.1f;
	}
  }

  if(!isClient)
    for(int e = 1; e < ships.size(); ++e)
      ships[e]->update();
  else
  {

  }

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
  {
    if(isClient)
    {
      Ship* ship = ships[0];
      const vector<int>& v = ship->getMessage();
      if(v.size() > 1)
      {
        int re = cl_socket->send_to(boost::asio::buffer(v), *receiver_endpoint);
        ship->clearMessage();
      }
    }
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
  (offset * cam_hist.front()).loadToGL();
  cam_hist.pop_front();
  cam_hist.push_back(center);

  glUseProgram(program);
  draw_meridians();

  cube.draw();
  Projectile::draw_all();
  glUseProgram(program);
  for(int i = 0; i < ships.size(); ++i)
    ships[i]->draw();

  MiniMap::draw(0, this, center);
}

void
NetWorld::fill_minimap()
{
  cube.draw_in_minimap();
  for(int i = 1; i < ships.size(); ++i)
    ships[i]->draw_in_minimap();
}
