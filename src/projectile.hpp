#pragma once

#include "matrix4.hpp"
#include "vector4.hpp"
#include "drawable.hpp"
#include "ship.hpp"

class Projectile: public Drawable
{
public:
  static void initialize();
  static void shot(Ship *s, const Matrix4& from, float speed);
  static void draw_all();
  static void draw_in_minimap();
  static void update_all();
  static bool collide(const Vector4& position, float radius);
  static bool collide(Ship *s);

private:
  Projectile(Ship *s, const Matrix4& from, float speed);
  void draw();
  void update();
  inline bool dead()
  {
    // Maximum Time To Live
    return ttl >= max_ttl;
  }

  Matrix4 ds;
  Ship *owner;
  unsigned short ttl;
  unsigned short max_ttl;
};
