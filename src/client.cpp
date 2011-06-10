#include <list>

#include "drawable.hpp"
#include "protocol.hpp"

#include "client.hpp"

extern std::list<Drawable*> drawable_objs;
extern int WIDTH;
extern int HEIGHT;

Client::Client()
{
  //ship = (Ship*)Drawable::create_ship();
}

void
Client::move(bool f, bool state)
{
  float accel;

  if(f)
  {
    accel = !state ? -0.00002 : 0.0;
  }
  else
  {
    accel = !state ? 0.00002 : 0.0;
  }

  ship->move(accel);
}

void
Client::motion(int x, int y)
{
  x -= WIDTH / 2;
  y -= HEIGHT / 2;

  ship->rotate(float(x) / float(WIDTH / 2), -(float(y) / float(HEIGHT / 2)));
}

void
Client::parseMessage(boost::array<int, 1024> msg, unsigned int bytes)
{
  unsigned int nums = bytes / sizeof(int);
  unsigned int i = 0;
  int x, y;
  while(i < nums)
  {
    switch(msg[i])
    {
    case QUIT:
      //drawable_objs.remove(ship);
      break;
    case TURN:
      x = msg[++i];
      y = msg[++i];
      motion(x, y);
      break;
    case MOVE_F:
      move(true, (msg[++i] == 0));
      break;
    case MOVE_B:
      move(false, (msg[++i] == 0));
      break;
    case SHOT:
      ship->shot(msg[++i] == 0);
      break;
    case UP:
      ship->move_up(msg[++i] == 0);
      break;
    case DOWN:
      ship->move_down(msg[++i] == 0);
      break;
    case LEFT:
      ship->move_left(msg[++i] == 0);
      break;
    case RIGHT:
      ship->move_right(msg[++i] == 0);
      break;
    case SPIN_L:
      ship->move_spinl(msg[++i] == 0);
      break;
    case SPIN_R:
      ship->move_spinr(msg[++i] == 0);
      break;
    }
    ++i;
  }   
}

