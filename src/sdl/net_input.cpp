#include <algorithm>

#include "protocol.hpp"
#include "options.hpp"
#include "net_world.hpp"
#include "game.hpp"

#include "net_input.hpp"

extern World* world;

using namespace std;
using namespace Options;

namespace NetInput
{

static Ship* ship;

void
set_ship(Ship* s)
{
  ship = s;
}

void
move(bool f, bool state)
{
  float accel;

  if(f)
  {
	accel = state ? -0.00002 : 0.0;
	ship->move_forward(state);
  }
  else
  {
	accel = state ? 0.00002 : 0.0;
	ship->move_backward(state);
  }

  ship->move(accel);
}

void
motion(int x, int y)
{
  x -= width / 2;
  y -= height / 2;

  ship->rotate(float(x) / float(width / 2), -(float(y) / float(height / 2)));
}

void
parse_message(const boost::array<float, 1024>& msg, unsigned int bytes, bool isClient)
{
  NetWorld* w = (NetWorld*)Game::world;
  boost::array<float, 1024>::const_iterator it;
  unsigned int nums = bytes / sizeof(int);
  unsigned int i = 0;
  int s_id = -1;
  Matrix4 t;
  int x, y;

  cout << "Received " << bytes << " bytes" << endl;
  for(int i = 0; i < nums; ++i)
    cout << int(msg[i]) << ' ';
  cout << endl;

  if(isClient)
  {
    s_id = int(msg[i++]);
    const vector<Ship*>& s_list = w->ships_list();
    if(s_id < s_list.size())
	  NetInput::set_ship(s_list[s_id]);
  }
  while(i < nums)
  {
    switch(int(msg[i]))
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
    case NEW_SHIP:
	  it = msg.begin() + i + 1;
	  copy(it, it + 16, &t[0][0]);
	  i += 16;
	  NetInput::set_ship(w->next_ship(t));
	  break;
    case INIT_POS:
      it = msg.begin() + i + 1;
	  copy(it, it + 16, &t[0][0]);
	  i += 16;
	  ship->setTransformation(t);
	  break;
    case UPDATE_SHIP:
      cout << "UPDATE CLIENT " << s_id << endl;
	  it = msg.begin() + i + 1;
	  copy(it, it + 16, &t[0][0]);
	  i += 16;
	  w->set_interpolation(s_id, ship->transformation(), t);
	  break;
    default:
      std::cout << "Error: protocol doesn't know what to do with header '" << msg[i] << '.' << std::endl;
      break;
    }
    ++i;
  }
}

}
