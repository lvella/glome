#pragma once

#include "object.hpp"
#include "drawable.hpp"
#include <memory>

class Renderer;

namespace MiniMap
{
  extern GLint proj_has_tex;

/*
 * @param wstart Start width of minimap
 * @param world Use to draw minimap in world
 * @param center Center of minimap
 */
  void draw(int wstart, int hstart, Renderer* world, const QRot& center,
      const std::vector<std::shared_ptr<Glome::Drawable>>& objs);
  void draw_dot(const Object &obj);
  void update();
  void initialize();
}
