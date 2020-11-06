#pragma once

#include "object.hpp"
#include "drawable.hpp"
#include "renderer.hpp"

namespace MiniMap
{
  extern GLint proj_has_tex;

/*
 * @param wstart Start width of minimap
 * @param world Use to draw minimap in world
 * @param center Center of minimap
 */
  void draw(MapRenderer* renderer, const QRot& inv_cam_t, const Renderer::ObjSet& objs);
  void draw_dot(const Object &obj);
  void update();
}
