#pragma once

class World;

namespace MiniMap
{
  void draw(int wstart, World* world, const Matrix4& center);
  void update();
  void initialize();
}
