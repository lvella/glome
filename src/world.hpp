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
  int iter;
  Ship ship[2];
  unsigned int points[2];
  RandomCube cube;

  // Camera transform historic
  std::deque<Matrix4> cam_hist[2];
};
