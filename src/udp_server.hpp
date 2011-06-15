#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "matrix4.hpp"

class Client;

namespace Server
{
  void start_receive();
  void handle_socket(const boost::system::error_code& error, std::size_t bytes);
  void handle_timer(const boost::system::error_code& error);
  void update_clients();
  void initialize(short int port, int threads = 1);
  void set_timer();
  void send_to_client(const std::vector<float>&, Client *);
  void send_to_client(Client *);
  void sync_client_world(Client* cl, bool update = true);
}
