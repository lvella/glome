#pragma once

#include "object.hpp"

class Drawable: public Object
{
public:
  Drawable(): Object() {}
  Drawable(const Matrix4& from): Object(from) {}
  virtual void update() {}
  virtual void draw() {}
  virtual bool dead() { return false; }
  static void update_all();
  static void draw_all();
  static Drawable* create_ship();
  static Drawable* create_random_cube();
};

