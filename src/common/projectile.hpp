#pragma once

#include "matrix4.hpp"
#include "vector4.hpp"
#include "object.hpp"
#include "ship.hpp"

class Projectile: public Object
{
public:
  static void initialize();
  static void shot(Ship *s, const Matrix4& from, float speed);
  static void draw_all(const Matrix4& cam);
  static void draw_in_minimap();
  static void update_all(const Vector4& camera_pos);
  static bool collide(const Vector4& position, float radius);
  static bool collide(Ship *s);

  bool operator<(const Projectile& other) const
  {
    return order_dist < other.order_dist;
  }

private:
  Projectile(Ship *s, const Matrix4& from, float speed);
  void draw(const Matrix4& cam);
  void update(const Vector4& camera_pos);
  inline bool dead()
  {
    // Maximum Time To Live
    return ttl >= max_ttl;
  }

  inline void die()
  {
    ttl = max_ttl;
  }

  Matrix4 ds;
  Ship *owner;
  unsigned short ttl;
  unsigned short max_ttl;
  unsigned short max_ttl_2;
  unsigned char alpha;

  float order_dist;
};
