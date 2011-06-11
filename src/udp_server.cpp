#include <map>
#include <iostream>

#include "client.hpp"
#include "world.hpp"

#include "udp_server.hpp"

extern boost::asio::io_service gIOService;

using namespace std;
using boost::asio::ip::udp;

namespace Server
{

boost::asio::ip::udp::socket* socket;
udp::endpoint remote_endpoint;
boost::array<int, 1024> recv_buf;
boost::asio::deadline_timer* timer;
map<udp::endpoint*, Client*> cl_map;
const int pkg_lim = 2;
int pkg_count;

void
start_receive()
{
  socket->async_receive_from(boost::asio::buffer(recv_buf),
                      remote_endpoint,
                      handle_socket);

  timer->expires_from_now(boost::posix_time::seconds(pkg_lim));
  timer->async_wait(handle_timer);
  gIOService.poll();
}

void
handle_socket(const boost::system::error_code& error, std::size_t bytes)
{
  if(!error)
  {
    Client* cl;
    timer->cancel();
    gIOService.poll();
    string addr = remote_endpoint.address().to_string();
    cout << "Received package from " << addr << endl;
    map<udp::endpoint*, Client*>::iterator it;
    if((it = cl_map.find(&remote_endpoint)) == cl_map.end())
    {
      cl = new Client();
      cl_map[&remote_endpoint] = cl;
    }
    else
      cl = it->second;

    cl->parseMessage(recv_buf, bytes);
/*
      cout << "Received " << bytes << " bytes from " << addr << ": ";
      int nums = bytes / sizeof(int);
      for(int i = 0; i < nums; ++i)
      {
        cout << recv_buf[i] << ' ';
      }
      cout << endl;
*/

    if(++pkg_count == pkg_lim)
      update_clients();
  }
}

void
handle_timer(const boost::system::error_code& error)
{
  if(!error)
  {
    if(pkg_count > 0)
    {
      update_clients();
      pkg_count = 0;
    }
  }
}

void
update_clients()
{
  boost::array<float, 16> send_buf;
  Client* cl;
  int i, j, k;
  map<udp::endpoint*, Client*>::iterator it = cl_map.begin();
  
  for(++it; it != cl_map.end(); ++it)
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
}

void
initialize(short int port)
{
  pkg_count = 0;
  socket = new boost::asio::ip::udp::socket(gIOService, udp::endpoint(udp::v4(), port));
  boost::asio::socket_base::non_blocking_io command(true);
  socket->io_control(command);
  timer = new boost::asio::deadline_timer(gIOService);
  gIOService.run();
}

}
