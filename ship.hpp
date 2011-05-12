#pragma once

#include "matrix4.hpp"

class Ship
{
public:
  static void initialize();

  Ship();
  void draw();
  void update(Matrix4 &old_rot);
  inline void rotate(float rx, float ry)
  {
    h_tilt = rx * 50.0 * M_PI / 180.0;
    v_tilt = ry * 37.0 * M_PI / 180.0;
  }

private:
  float v_tilt, h_tilt;
  Matrix4 rot;
};
