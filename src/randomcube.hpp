#pragma once

#include "drawable.hpp"

class RandomCube: public Drawable
{
public:
  RandomCube();

  void draw();
  void randomize();
};
