#pragma once

#include "object.hpp"

class Drawable: public Object
{
public:
  Drawable(): Object() {}
  Drawable(const Matrix4& from): Object(from) {}
  virtual ~Drawable();
  virtual void draw() = 0;
  void draw_in_minimap();
};
