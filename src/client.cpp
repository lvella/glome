#include <iostream>
#include <list>

#include "drawable.hpp"
#include "protocol.hpp"
#include "udp_server.hpp"
#include "net_world.hpp"
#include "4dmath.hpp"

#include "client.hpp"

extern int WIDTH;
extern int HEIGHT;

extern World* world;

using namespace std;
using boost::asio::ip::udp;

Client::Client(udp::endpoint* end)
{
  last_msg = 0;
  remote_endpoint = end;
  ship = ((NetWorld*)world)->next_ship(
		  /*
        xy_matrix(rand()/10000.0f)
      * xz_matrix(rand()/10000.0f)
      * yz_matrix(rand()/10000.0f)
      * xw_matrix(rand()/10000.0f)
      * yw_matrix(rand()/10000.0f)
      * zw_matrix(rand()/10000.0f)
      */
		  Matrix4::IDENTITY);
  make_init_pos_msg();
  Server::send_to_client(message, this);
}

void
Client::move(bool f, bool state)
{
  float accel;

  if(f)
  {
    accel = !state ? -0.00002 : 0.0;
  }
  else
  {
    accel = !state ? 0.00002 : 0.0;
  }

  ship->move(accel);
}

void
Client::motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  ship->rotate(float(x) / float(WIDTH / 2), -(float(y) / float(HEIGHT / 2)));
}

void
Client::make_update_ship_msg(const Matrix4& t, unsigned int id)
{
  message.push_back(UPDATE_SHIP);
  message.push_back(id);
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      message.push_back(t[i][j]);
}

void
Client::make_new_ship_msg(const Matrix4& t, unsigned int id)
{
  message.push_back(NEW_SHIP);
  message.push_back(id);
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      message.push_back(t[i][j]);
}

void
Client::make_init_pos_msg()
{
  const Matrix4& t = ship->transformation();
  message.push_back(INIT_POS);
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      message.push_back(t[i][j]);
}

void
Client::parseMessage(boost::array<int, 1024> msg, unsigned int bytes)
{
  unsigned int nums = bytes / sizeof(int);
  unsigned int i = 0;
  int x, y;
/*
  for(int i = 0; i < nums; ++i)
    cout << msg[i] << ' ';
  cout << endl;
*/
  last_msg = msg[i++];
  while(i < nums)
  {
    switch(msg[i])
    {
    case QUIT:
      //drawable_objs.remove(ship);
      break;
    case TURN:
      x = msg[++i];
      y = msg[++i];
      motion(x, y);
      break;
    case MOVE_F:
      move(true, (msg[++i] == 0));
      break;
    case MOVE_B:
      move(false, (msg[++i] == 0));
      break;
    case SHOT:
      ship->shot(msg[++i] == 0);
      break;
    case UP:
      ship->move_up(msg[++i] == 0);
      break;
    case DOWN:
      ship->move_down(msg[++i] == 0);
      break;
    case LEFT:
      ship->move_left(msg[++i] == 0);
      break;
    case RIGHT:
      ship->move_right(msg[++i] == 0);
      break;
    case SPIN_L:
      ship->move_spinl(msg[++i] == 0);
      break;
    case SPIN_R:
      ship->move_spinr(msg[++i] == 0);
      break;
    default:
      std::cout << "Error: protocol doesn't know what to do with header '" << msg[i] << '.' << std::endl;
      break;
    }
    ++i;
  }
}

