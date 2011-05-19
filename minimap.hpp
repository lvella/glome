#pragma once

class MiniMap
{
public:
  MiniMap();

  void draw();
  void update();
  void initialize();
  void load_texture();

private:
  GLuint tex_minimap;
};
