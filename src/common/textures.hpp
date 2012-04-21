#pragma once

#include "gl.hpp"
#include "vector2.hpp"

void create_circle_texture(int w, float p, int a0, int a1, GLuint& tex, bool gen_mipmap = false);
void create_spherical_texture(int size, GLuint& tex);
GLuint create_noise_texture(int w, int h, float scale, const Vector2& offset);
