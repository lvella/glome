#pragma once

#include "object.hpp"

class RandomCube: public Object
{
public:
  RandomCube();

  void draw();
  void randomize();
};
