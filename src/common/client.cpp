#include "object.hpp"
#include "protocol.hpp"
#include "udp_server.hpp"
#include "net_world.hpp"
#include "math.hpp"

#include "client.hpp"

extern int width;
extern int height;

namespace Game {
extern World* world;
}

using namespace std;
using boost::asio::ip::udp;

Client::Client(udp::endpoint* end)
{
  last_msg = 0;
  remote_endpoint = end;
  ship = ((NetWorld*)Game::world)->next_ship(
		  /*
        xy_matrix(rand()/10000.0f)
      * xz_matrix(rand()/10000.0f)
      * yz_matrix(rand()/10000.0f)
      * xw_matrix(rand()/10000.0f)
      * yw_matrix(rand()/10000.0f)
      * zw_matrix(rand()/10000.0f)
      */
		  Matrix4::IDENTITY);
  id = ((NetWorld*)Game::world)->ships_list().size() - 1;
  make_init_pos_msg();
  Server::send_to_client(message, this);
}

void
Client::make_update_ship_msg(const Matrix4& t, bool ithis)
{
  message.push_back(ithis ? 0 : id);
  message.push_back(UPDATE_SHIP);
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      message.push_back(t[i][j]);
}

void
Client::make_new_ship_msg()
{
  message.clear();
  const Matrix4& t = ship->transformation();
  message.push_back(NEW_SHIP);
  copy(&t[0][0], &t[3][3], message.begin());
}

void
Client::make_init_pos_msg()
{
  const Matrix4& t = ship->transformation();
  message.push_back(0);
  message.push_back(INIT_POS);
  for(int i = 0; i < 4; ++i)
    for(int j = 0; j < 4; ++j)
      message.push_back(t[i][j]);
}
