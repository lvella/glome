#pragma once

#include "matrix4.hpp"

class Ship
{
public:
  static void initialize();

  Ship();
  void draw();
  void update();
  inline void rotate(float rx, float ry)
  {
    h_req = rx / 150.0;
    v_req = ry / 150.0;
    /*h_req = rx * 50.0 * M_PI / 180.0;
    v_req = ry * 37.0 * M_PI / 180.0;*/
  }
  inline void move(float s)
  {
    speed = s;
  }

private:
  float v_req, h_req;
  float v_tilt, h_tilt;
  float speed;

  Matrix4 t;
};
