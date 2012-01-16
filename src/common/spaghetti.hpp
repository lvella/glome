#pragma once

#include "updatable.hpp"
#include "drawable.hpp"

#define SPAGHETTI_COUNT 100 // Number of cubic Bézier curves

class Spaghetti: public Updatable, public Glome::Drawable 
{
public:
  Spaghetti();
  virtual ~Spaghetti();

  void draw(Camera& s);
  void update();
private:
  Matrix4 velo;

  Vector4 bezier[(SPAGHETTI_COUNT * 3)+2];
  union {
    struct {
      GLuint ibo, vbo;
    };
    GLuint bufobjs[2];
  };
};
