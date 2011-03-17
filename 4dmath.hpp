#pragma once

#include "matrix4.hpp"

inline Matrix4 ahead_matrix(float angle) {
  const float c = cos(angle);
  const float s = sin(angle);
  return Matrix4(1,0,0,0,
		 0,1,0,0,
		 0,0,c,-s,
		 0,0,s,c);
}

inline Matrix4 up_matrix(float angle) {
  const float c = cos(angle);
  const float s = sin(angle);
  return Matrix4(1,0,0,0,
		 0,c,0,-s,
		 0,0,1,0,
		 0,s,0,c);
}
