#pragma once

#include "object.hpp"
#include "mesh.hpp"

class Asteroid: public Object
{
public:
  Asteroid();
  ~Asteroid();
  void draw();
  void update();

private:
  Mesh* mesh;
};

