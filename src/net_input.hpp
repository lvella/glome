#pragma once

#include <boost/array.hpp>

#include "ship.hpp"

namespace NetInput
{
  void set_ship(Ship* s);
  void mouse_motion(int x, int y);
  void mouse_button(int button, int state);
  void parse_message(const boost::array<float, 1024>& msg, unsigned int bytes, bool isClient);
}

