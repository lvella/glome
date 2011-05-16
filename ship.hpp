#pragma once

#include <SDL.h>

#include "matrix4.hpp"

class Ship
{
public:
  static void initialize();

  Ship();
  void draw();
  void update();
  void shot();
  inline void rotate(float rx, float ry)
  {
    h_req = rx / 100.0;
    v_req = -ry / 100.0;
  }
  inline void move(float a)
  {
    accel = a;
  }

  inline const Matrix4& transformation() const {
    return t;
  }

private:
  float v_req, h_req;
  float v_tilt, h_tilt;
  float accel, speed;
  int sps;
  Uint32 last_shot;

  Matrix4 t;
};
