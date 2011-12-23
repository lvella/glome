#pragma once

#include "updatable.hpp"
#include "drawable.hpp"

class RandomCube: public Updatable, public drawable::Drawable 
{
public:
  RandomCube();

  void draw(const Shader& s);
  void randomize();
};
