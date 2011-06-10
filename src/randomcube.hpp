#pragma once

#include "drawable.hpp"

class RandomCube: public Drawable
{
public:
  RandomCube();

  void update();
  void draw();

private:
  void randomize();
};
