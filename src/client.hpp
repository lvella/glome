#pragma once

#include <boost/array.hpp>

#include "ship.hpp"

class Client
{
public:
  Client();
  ~Client();

  void move(bool f, bool state);
  void motion(int x, int y);
  void parseMessage(boost::array<int, 1024>, unsigned int);
  Ship* getShip() { return ship; }

private:
  Ship* ship;
};

