#pragma once

#include <GL/glew.h>
#include <cmath>

class Matrix4;

class Vector4
{
public:
  inline Vector4(float xl, float yl, float zl, float wl)
  {
    x = xl;
    y = yl;
    z = zl;
    w = wl;
  }

  Vector4 operator+(const Vector4& ref)
  {
    return Vector4(x + ref.x, y + ref.y, z + ref.z, w + ref.w);
  }

  Vector4 operator-(const Vector4& ref)
  {
    return Vector4(x-ref.x, y-ref.y, z-ref.z, w-ref.w);
  }

  float length()
  {
    return sqrt(x*x + y*y + z*z + w*w);
  }

  void loadVertex()
  {
    glVertex4fv(v);
  }

private:
  friend class Matrix4;

  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };
    float v[4];
  };
};

