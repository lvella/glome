#pragma once

#include "object.hpp"

#define SPAGHETTI_COUNT 100 // Number of cubic Bézier curves

class Spaghetti: public Object
{
public:
  Spaghetti();

  void draw();
  void update();
private:
  Matrix4 velo;

  Vector4 p[(SPAGHETTI_COUNT * 3)+2];
};
