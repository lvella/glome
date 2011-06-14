#pragma once

class World
{
public:
  World() {}
  virtual bool update() = 0;
  virtual void draw() = 0;
  virtual void fill_minimap() = 0;
};
