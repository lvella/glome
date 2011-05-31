#pragma once

#include "ship.hpp"

class Client
{
public:
  Client();
  ~Client();

  void parseMessage(const char *);
  Ship* getShip() { return ship; }

private:
  Ship* ship;
};

