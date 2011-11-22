#pragma once

#include "matrix4.hpp"
#include "shader.hpp"

class Object
{
public:
  Object(const Matrix4& init = Matrix4::IDENTITY):
    t(init)
  {}
  virtual ~Object() = 0;

  virtual void draw(const Shader& cam) = 0;

  const Matrix4& transformation() const
  {
    return t;
  }

  void setTransformation(const Matrix4& ref)
  {
    t = ref;
  }

protected:
  Matrix4 t;
};
