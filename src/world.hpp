#pragma once

#include <deque>
#include "ship.hpp"
#include "randomcube.hpp"
#include "matrix4.hpp"

class World
{
public:
  static void initialize();

  World();

  bool update();
  void draw();
  void fill_minimap();

private:
  Ship ship;
  RandomCube cube;

  // Camera transform historic
  std::deque<Matrix4> cam_hist;
};
