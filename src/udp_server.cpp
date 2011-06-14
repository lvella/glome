#include <map>
#include <iostream>

#include <boost/thread.hpp>

#include "client.hpp"
#include "net_world.hpp"
#include "protocol.hpp"

#include "udp_server.hpp"

extern boost::asio::io_service gIOService;
extern NetWorld* world;

using namespace std;
using boost::asio::ip::udp;

namespace Server
{

boost::asio::ip::udp::socket* socket;
udp::endpoint remote_endpoint;
boost::array<int, 1024> recv_buf;
boost::asio::deadline_timer* timer;
map<string, Client*> end_cl_map;
const int pkg_lim = 5;
const int delta = 1000; // in ms
int pkg_count;

void
set_timer()
{
  timer->expires_from_now(boost::posix_time::milliseconds(delta));
  timer->async_wait(handle_timer);
  gIOService.poll();
}

void
start_receive()
{
  socket->async_receive_from(boost::asio::buffer(recv_buf),
                      remote_endpoint,
                      handle_socket);
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
sync_client_world(Client* cl)
{
  const vector<Ship*>& ships = world->ships_list();
  Ship* cl_s = cl->get_ship();
  unsigned int id = 0;
  for(int i = 0; i < ships.size(); ++i)
  {
    if(ships[i] == cl_s)
      id = i;
    else
      cl->make_new_ship_msg(ships[i]->transformation(), (i < id) ? (i + 1) : id);
  }
  send_to_client(cl);
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
      sync_client_world(cl);
    }
    else
    {
      cl = it->second;
      cl->parseMessage(recv_buf, bytes);
    }

    if(++pkg_count == pkg_lim)
    {
      pkg_count = 0;
      timer->cancel();
      gIOService.poll();
      //update_clients();
      set_timer();
    }

    start_receive();
  }
}

void
handle_timer(const boost::system::error_code& error)
{
  if(!error)
  {
    if(pkg_count > 0)
    {
      //update_clients();
      pkg_count = 0;
    }
    set_timer();
  }
}

void
update_clients()
{
/*
  boost::array<float, 16> send_buf;
  Client* cl;
  int i, j, k;
  map<udp::endpoint*, Client*>::iterator it = end_cl_map.begin();
  
  for(++it; it != end_cl_map.end(); ++it)
  {
    cl = it->second;
    Matrix4 t = cl->getShip()->transformation();
    for(i = 0, k = 0; i < 4; ++i)
      for(j = 0; j < 4; ++j, ++k)
        send_buf[k] = t[i][j];

    boost::system::error_code ignored_error;
    socket->send_to(boost::asio::buffer(send_buf),
                   remote_endpoint,
                   0,
                   ignored_error);
  }
*/
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
