#pragma once

#include <boost/asio.hpp>

namespace Server
{
  void start_receive();
  void handle_socket(const boost::system::error_code& error, std::size_t bytes);
  void handle_timer(const boost::system::error_code& error);
  void update_clients();
  void initialize(short int port);
}
