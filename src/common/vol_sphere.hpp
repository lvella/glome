#pragma once

#include <functional>
#include "math.hpp"
#include "object.hpp"

// This class represents a sphere for the
// purposes of collision detection.

// TODO: make object center different from sphere center.
// This is useful for asymetric stuff, like engine fire.
class VolSphere: virtual public Object
{
  public:
    VolSphere() = default;

    // Build a sphere from radius.
    // See set_radius().
    VolSphere(float radius)
    {
      set_radius(radius);
    }

    // Set sphere radius. As usual for this game space,
    // the radius, as any distance, must be given in radians.
    // The radius must be given in range [0, PI)
    void set_radius(float r);

    // Tells if this intersects with a given great sphere.
    bool intersects_great_sphere(const Vector4& center) const
    {
      return center.dot(get_world_pos()) >= cos_great_dist;
    }

    // Tells if two given sphere intersects
    bool intersects(const VolSphere& other, float &cos_dist) const;
    bool intersects(const VolSphere& other) const;

    // Tells if a given point is contained in the sphere
    bool contains(const Vector4 &p) const
    {
      return get_world_pos().dot(p) < std::cos(radius);
    }

    float get_radius() const
    {
      return radius;
    }

  private:
    float radius = 0.0;

    // Great spheres (sphere of radius π) are a special case
    // because they are used extensively by the spheric Octree
    // algorithm, so it makes sense to have the value precomputed.
    // This is the cosine of the sum of the radius of this sphere
    // and the radius of a great sphere (π / 2).
    float cos_great_dist;
};
