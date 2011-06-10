#pragma once

#include <cstring>

#include <vector>

#include "protocol.hpp"
#include "drawable.hpp"

class Ship: public Drawable
{
public:
  static void initialize();

  Ship();
  void draw();
  void update();

  inline void rotate(float rx, float ry)
  {
    h_req = rx / 100.0;
    v_req = -ry / 100.0;
  }

  inline void move(float a)
  {
    accel = a;
  }

  inline void motion(int x, int y)
  {
    motion_x = x;
    motion_y = y;
    message.push_back(TURN);
    message.push_back(x);
    message.push_back(y);
  }

  inline void move_forward(bool a)
  {
    forward = a;
    message.push_back(MOVE_F);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_backward(bool a)
  {
    backward = a;
    message.push_back(MOVE_B);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_up(bool a)
  {
    up = a;
    message.push_back(UP);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_down(bool a)
  {
    down = a;
    message.push_back(DOWN);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_left(bool a)
  {
    left = a;
    message.push_back(LEFT);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_right(bool a)
  {
    right = a;
    message.push_back(RIGHT);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_spinl(bool a)
  {
    spinl = a;
    message.push_back(SPIN_L);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void move_spinr(bool a)
  {
    spinr = a;
    message.push_back(SPIN_R);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void shot(bool a)
  {
    sh = a;
    message.push_back(SHOT);
    message.push_back(a ? COMMAND_BEGIN : COMMAND_END);
  }

  inline void quit()
  {
    q = true;
    message.push_back(QUIT);
  }

  const std::vector<int>& getMessage() const { return message; }

  void clearMessage() { message.clear(); }

private:
  inline static void handle_commands(bool, bool, float &, float &, float);
  inline static void accelerate(float &, float &, float);

  float v_req, h_req;
  float v_tilt, h_tilt;
  float accel, speed;
  float speed_v;
  float speed_h;
  float speed_s;
  int motion_x;
  int motion_y;
  bool forward;
  bool backward;
  bool up;
  bool down;
  bool left;
  bool right;
  bool spinl;
  bool spinr;
  bool sh;
  bool q;
  int sps;
  int shot_count;
  std::vector<int> message;

  static Matrix4 r_canon, l_canon;
};

