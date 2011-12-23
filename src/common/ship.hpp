#pragma once

#include "updatable.hpp"
#include "drawable.hpp"

#include "engine.hpp"
#include "mesh.hpp"
#include "ship_controller.hpp"

class Ship : public Updatable, public drawable::Drawable 
{
public:
	Ship(MeshTypes type = HUNTER);
	~Ship() {};
	void draw(const Shader& s);
	void draw(const Shader& s,Matrix4 cam, Matrix4 proj);
	void update();
	void load_guns(MeshTypes type);
	void set_controller(ShipController* pctrl);

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

  const std::vector<float>& getMessage() const { return message; }

  inline void clearMessage()
  {
    message.clear();
    //message.push_back(++msg_id);
  }

protected:
	Mesh* mesh;
	ShipController* ctrl;
	Matrix4 r_canon, l_canon;
	ship_gun gun_l;
	ship_gun gun_r;
	uint16_t nguns;
	Engine* engine;
};

