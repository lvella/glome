#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "vol_sphere.hpp"

class Spaghetti:
  public Updatable,
  public Glome::Drawable,
  public VolSphere<Object>
{
public:
  Spaghetti();
  virtual ~Spaghetti();

  void draw(Camera& s);
  void update();
private:
  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

  Matrix4 velo;
  GLuint vbo;
  GLsizei count;
};
