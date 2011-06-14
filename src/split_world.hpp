#pragma once

#include <deque>

#include "ship.hpp"
#include "randomcube.hpp"
#include "matrix4.hpp"
#include "world.hpp"

class SplitWorld: World
{
public:
  static void initialize();

  SplitWorld();

  bool update();
  void draw();
  void fill_minimap();
  Ship* main_ship() { return &ship[0]; }

private:
  int iter;
  Ship ship[2];
  RandomCube cube;

  // Camera transform historic
  std::deque<Matrix4> cam_hist[2];
};
