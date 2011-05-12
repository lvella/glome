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
    h_req = rx * 50.0 * M_PI / 180.0;
    v_req = ry * 37.0 * M_PI / 180.0;
  }

private:
  float v_req, h_req;
  float v_tilt, h_tilt;
  Matrix4 rot;
};
