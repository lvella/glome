#pragma once

#include <boost/asio.hpp>
#include <iostream>

#include "world.hpp"

extern const int WIDTH;
extern const int HEIGHT;

extern const float FOV;

extern boost::asio::io_service gIOService;
extern bool isServer;
extern bool isClient;
extern short port;
extern World* world;
extern bool isSplit;
extern std::string host;

