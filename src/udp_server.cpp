#include <algorithm>
#include <iostream>
#include <map>

#include <boost/thread.hpp>

#include "client.hpp"
#include "net_world.hpp"
#include "protocol.hpp"
#include "net_input.hpp"

#include "udp_server.hpp"

extern boost::asio::io_service gIOService;
extern World* world;

using namespace std;
using boost::asio::ip::udp;

namespace Server
{

static NetWorld* world = (NetWorld*)::world;
float buf[1025];
boost::asio::ip::udp::socket* socket;
udp::endpoint remote_endpoint;
boost::array<float, 1024> recv_buf;
boost::asio::deadline_timer* timer;
map<string, Client*> end_cl_map;
const int pkg_lim = 200000000;
const int delta = 5000; // in ms
int pkg_count;

void
set_timer()
{
  timer->expires_from_now(boost::posix_time::milliseconds(delta));
  timer->async_wait(handle_timer);
}

void
start_receive()
{
  socket->async_receive_from(boost::asio::buffer(recv_buf),
                      remote_endpoint,
                      handle_socket);
}

inline void
send_to_client(const boost::array<float, 1024>& message, unsigned int bytes, Client* cl)
{
  boost::system::error_code ignored_error;
  socket->send_to(boost::asio::buffer(message, bytes),
                 cl->target(),
                 0,
                 ignored_error);
  cl->clear_message();
}

void
send_to_client(const vector<float>& message, Client* cl)
{
  boost::system::error_code ignored_error;
  socket->send_to(boost::asio::buffer(message),
                 cl->target(),
                 0,
                 ignored_error);
  cl->clear_message();
}

void
send_to_client(Client* cl)
{
  const vector<float>& v = cl->get_message();
  boost::system::error_code ignored_error;
  socket->send_to(boost::asio::buffer(cl->get_message()),
                 cl->target(),
                 0,
                 ignored_error);
  cl->clear_message();
}

void
send_ships_client(Client* cl)
{
  int i, j, k, u;
  const vector<Ship*>& ships = world->ships_list();
  vector<float> msg;
  Ship* cl_s = cl->get_ship();
  for(int i = 0; i < ships.size() - 1; ++i)
  {
	const Matrix4& t = ships[i]->transformation();
    msg.push_back(i + 1);
    msg.push_back(NEW_SHIP);
    for(j = 0, u = 2; j < 4; ++j)
      for(k = 0; k < 4; ++k, ++u)
     msg.push_back(t[j][k]);
     //copy(&t[0][0], &t[3][3], msg.begin() + 2);
     send_to_client(msg, cl);
     msg.clear();
  }
}

void
broadcast_new_ship(Client* cl)
{
  cl->make_new_ship_msg();
  const vector<float>& msg = cl->get_message();
  send_to_all(msg, msg.size() * sizeof(float), cl->get_id(), true);
  cl->clear_message();
}

void
broadcast_sync()
{
  int i, j, k;
  const vector<Ship*>& ships = world->ships_list();
  vector<float> msg;
  for(int i = 0; i < ships.size(); ++i)
  {
	const Matrix4& t = ships[i]->transformation();
	msg.push_back(UPDATE_SHIP);
	for(j = 0; j < 4; ++j)
	  for(k = 0; k < 4; ++k)
	 msg.push_back(t[j][k]);
	 //copy(&t[0][0], &t[3][3], msg.begin() + 2);
	 send_to_all(msg, msg.size() * sizeof(float), i, false);
	 msg.clear();
  }
}

void
handle_socket(const boost::system::error_code& error, std::size_t bytes)
{
  if(!error)
  {
    Client* cl;
    string addr = remote_endpoint.address().to_string();
    map<string, Client*>::iterator it;
    if((it = end_cl_map.find(addr)) == end_cl_map.end())
    {
      udp::endpoint* new_end = new udp::endpoint(remote_endpoint);
      cl = new Client(new_end);
      end_cl_map[addr] = cl;
      send_ships_client(cl);
      broadcast_new_ship(cl);
    }
    else
    {
      cl = it->second;
      send_to_all(recv_buf, bytes, cl->get_id(), true);
      NetInput::set_ship(cl->get_ship());
      NetInput::parse_message(recv_buf, bytes, false);
    }

    start_receive();
  }
}

void
handle_timer(const boost::system::error_code& error)
{
  if(!error)
  {
    broadcast_sync();
    set_timer();
  }
  else
    ;//cout << "TIMER ERROR: " << error.message().data() << endl;
}

void
initialize(short int port, int threads)
{
  pkg_count = 0;
  socket = new boost::asio::ip::udp::socket(gIOService, udp::endpoint(udp::v4(), port));
  timer = new boost::asio::deadline_timer(gIOService);
  start_receive();
  set_timer();
  for(int i = 0; i < threads; ++i)
    boost::thread service(boost::bind(&boost::asio::io_service::run, &gIOService));
}

}
