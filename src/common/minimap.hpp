#pragma once

#include "object.hpp"


class World;

namespace MiniMap
{
	/*
 * @param wstart Start width of minimap
 * @param world Use to draw minimap in world
 * @param center Center of minimap 
 */
  void draw(int wstart, World* world, const Matrix4& center);
  void draw_dot(const Object &obj);
  void update();
  void initialize();
}
