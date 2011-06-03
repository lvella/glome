#pragma once

#include "matrix4.hpp"

// Rotations from viewpoint

// This one is almost like glRotate, but in radians, and the vector must be normalized.
Matrix4 rotation(float angle, float x, float y, float z);

Matrix4 xy_matrix(float angle);
Matrix4 xz_matrix(float angle);
Matrix4 yz_matrix(float angle);

// "Translations" from viewpoint
Matrix4 xw_matrix(float angle);
Matrix4 zw_matrix(float angle);
Matrix4 yw_matrix(float angle);
