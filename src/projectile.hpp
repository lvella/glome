#pragma once

#include "matrix4.hpp"
#include "drawable.hpp"

class Projectile: public Drawable
{
public:
  static void initialize();
  static void shot(const Matrix4& from, const Matrix4& speed);
  static void draw_all();
  static void draw_in_minimap();
  static void update_all();

private:
  Projectile(const Matrix4& from, const Matrix4& speed);
  void draw();
  void update();
  inline bool dead()
  {
    // Maximum Time To Live
    return ttl >= max_ttl;
  }

  Matrix4 ds;
  unsigned short ttl;
  unsigned short max_ttl;
};
