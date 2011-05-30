#pragma once

#include <ctime>
#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

extern boost::asio::io_service gIOService;

class udp_server
{
public:
	~udp_server();

	udp_server(short int port);

  void update();

private:
  boost::asio::ip::udp::socket socket;
};

