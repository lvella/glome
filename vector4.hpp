#pragma once

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

  Vector4(const Vector4& ref)
  {
    x = ref.x;
    y = ref.y;
    z = ref.z;
    w = ref.w;
  }

  void operator=(const Vector4& ref)
  {
    x = ref.x;
    y = ref.y;
    z = ref.z;
    w = ref.w;
  }

//private:
  float x;
  float y;
  float z;
  float w;
  //float v[4];
};

