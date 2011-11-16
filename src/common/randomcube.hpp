#pragma once

#include "object.hpp"

class RandomCube: public Object
{
public:
  RandomCube();

  void draw(const Matrix4& cam);
  void randomize();
};
