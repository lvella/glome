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

  Vector4 operator+(const Vector4& ref) const
  {
    return Vector4(x + ref.x, y + ref.y, z + ref.z, w + ref.w);
  }

  Vector4 operator-(const Vector4& ref) const
  {
    return Vector4(x-ref.x, y-ref.y, z-ref.z, w-ref.w);
  }

  Vector4 operator*(float s) const
  {
    return Vector4(s*x, s*y, s*z, s*w);
  }

  float& operator[](size_t elem) {
    return v[elem];
  }

  float squared_length()
  {
    return x*x + y*y + z*z + w*w;
  }

  float length()
  {
    return sqrt(squared_length());
  }

  void loadVertex()
  {
    glVertex4fv(v);
  }

  /** Function for writing to a stream.
   */
  inline friend std::ostream& operator <<
  ( std::ostream& o, const Vector4& v )
  {
    o << v.v[0];
    for(size_t j = 1; j < 4; ++j)
      {
        o << ", " << v.v[j];
      }
    o << '\n';
    return o;
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

