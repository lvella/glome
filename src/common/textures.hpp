#pragma once

#include "gl.hpp"

void create_circle_texture(int w, float p, int a0, int a1, GLuint& tex, bool gen_mipmap = false);
void create_spherical_texture(int size, GLuint& tex);
