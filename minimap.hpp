#pragma once

#include <GL/glu.h>

class MiniMap
{
public:
  MiniMap();

  void draw();
  void update();
  void initialize();
  void create_circle_texture(int, float, int, int, GLuint&);

private:
  GLuint tex_minimap;
  GLuint tex_object;
};
