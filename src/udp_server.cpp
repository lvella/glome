#include <map>
#include <iostream>

#include <boost/thread.hpp>

#include "client.hpp"

#include "udp_server.hpp"

using namespace std;
using boost::asio::ip::udp;

map<string, Client*> cl_map;
extern std::list<Drawable*> drawable_objs;

void
udp_server::update()
{
  boost::array<float, 4> recv_buf;
  udp::endpoint remote_endpoint;
  boost::system::error_code error;
  unsigned int bytes;
  string addr;
  Client* cl;

  do
  {
    bytes = socket.receive_from(boost::asio::buffer(recv_buf),
                      remote_endpoint,
                      0,
                      error);
    if(bytes < 25)
    {
      cout << "Received " << bytes << " bytes." << endl;
    }
    else
    {
      addr = remote_endpoint.address().to_string();
      map<string, Client*>::iterator it;
      if((it = cl_map.find(addr)) == cl_map.end())
      {
        cl = new Client();
        cl_map[addr] = cl;
        drawable_objs.push_front(cl->getShip());
      }
      else
        cl = it->second;

      //cout << "Received from " << addr << ": ";
      //for(unsigned int i = 0; i < recv_buf.size(); ++i)
      //  cout << recv_buf[i] << ' ';
      //cout << endl;

    }

    // Process what to send back..

/*
    boost::system::error_code ignored_error;
    socket.send_to(boost::asio::buffer(message),
                   remote_endpoint,
                   boost::asio::transfer_all(),
                   ignored_error);
*/
  }
  while(bytes != 0);
}

udp_server::~udp_server()
{
}

udp_server::udp_server(short int port):
  socket(gIOService, udp::endpoint(udp::v4(), port))
{
  boost::asio::socket_base::non_blocking_io command(true);
  socket.io_control(command);
}
