#include "drawable.hpp"

#include "client.hpp"

Client::Client()
{
  ship = (Ship*)Drawable::create_ship();
}

void
parseMessage(const char* msg)
{
}

