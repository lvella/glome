#pragma once

#include "object.hpp"

class RandomCube: public Object
{
public:
  RandomCube();

  void draw(const Shader& s);
  void randomize();
};
