#pragma once

#include <deque>
#include "ship.hpp"
#include "asteroid.hpp"
#include "randomcube.hpp"
#include "matrix4.hpp"

#include "world.hpp"

class SplitWorld: public World
{
public:
  static void initialize();

  SplitWorld();

  virtual void update();
  virtual void draw();
  void fill_minimap();

private:
  int iter;
  Ship ship[2];
  unsigned int points[2];
  RandomCube cube;

  Asteroid a;

  // Camera transform historic
  std::deque<Matrix4> cam_hist[2];
};
