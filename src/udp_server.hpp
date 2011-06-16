#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "client.hpp"
#include "matrix4.hpp"

namespace Server
{
  void start_receive();
  void handle_socket(const boost::system::error_code& error, std::size_t bytes);
  void handle_timer(const boost::system::error_code& error);
  void update_clients();
  void initialize(short int port, int threads = 1);
  void set_timer();
  //void send_to_all(boost::array<float, 1024>& message, unsigned int bytes, unsigned int cl_id);
  inline void send_to_client(const boost::array<float, 1024>& message, unsigned int bytes, Client* cl);
  void send_to_client(const std::vector<float>&, Client *);
  void send_to_client(Client *);
  void sync_client_world(Client* cl, bool update = true);

  template <typename T>
  void
  send_to_all(T& message, unsigned int bytes, unsigned int cl_id)
  {
    extern float buf[1025];
	extern boost::asio::ip::udp::socket* socket;
	extern std::map<std::string, Client*> end_cl_map;

    int i = 0;
    boost::system::error_code ignored_error;
    std::map<std::string, Client*>::iterator it = end_cl_map.begin();
    std::copy(message.begin(), message.begin() + bytes, &buf[1]);
    for(; it != end_cl_map.end(); ++it, ++i)
    {
  	  Client* cl_aux = it->second;
  	  unsigned int id_aux = cl_aux->get_id();
      if(cl_aux->get_id() != cl_id)
      {
        buf[0] = (cl_id <= id_aux) ? (cl_id + 1) : cl_id;
        socket->send_to(boost::asio::buffer(buf, bytes + sizeof(int)),
                       cl_aux->target(),
                       0,
                       ignored_error);
      }
    }
  }
}
