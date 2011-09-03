#pragma once

#include "drawable.hpp"
#include "mesh.hpp"

class Asteroid: public Drawable
{
public:
  Asteroid();
  ~Asteroid();
  void draw();
  void update();

private:
  Mesh* mesh;
};

