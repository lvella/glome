#pragma once

#include "object.hpp"

class World;

namespace MiniMap
{
  void draw(int wstart, World* world, const Matrix4& center);
  void draw_dot(const Object &obj);
  void update();
  void initialize();
}
