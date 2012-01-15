#pragma once

#include "object.hpp"

class Renderer;

namespace MiniMap
{
	/*
 * @param wstart Start width of minimap
 * @param world Use to draw minimap in world
 * @param center Center of minimap 
 */
  void draw(int wstart, Renderer* world, const Matrix4& center);
  void draw_dot(const Object &obj);
  void update();
  void initialize();
}
