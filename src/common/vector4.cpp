#include <iostream>

#include "vector4.hpp"

const Vector4 Vector4::CANONICAL = Vector4(0.0f, 0.0f, 0.0f, -1.0f);

Vector4 Vector4::random_direction() {
    Vector4 ret;

    for(int i = 0; i < 4; ++i)
        ret.v[i] = float(rand());

    ret.normalize();

    return ret;
}

std::ostream&
operator<<(std::ostream& o, const Vector4& v)
{
  o << v.v[0];
  for(size_t j = 1; j < 4; ++j)
    {
      o << ", " << v.v[j];
    }
  o << '\n';
  return o;
}
