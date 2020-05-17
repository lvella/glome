#pragma once

#include <functional>
#include <cassert>
#include "math.hpp"
#include "object.hpp"

// This class represents a sphere for the
// purposes of collision detection.
class VolSphere: virtual private Object
{
  public:
    // Build a sphere from center and radius. As usual
    // for the spheric space, the center must be normalized,
    // and the radius, as any distance, must be given in radians.
    // The radius must be given in range [0, PI)
    VolSphere(float radius):
      radius(radius),
      cos_great_dist(std::cos(radius + math::pi_2))
    {
      assert(radius >= 0);
      assert(radius < math::pi);
    }

    // Tells if this intersects with a given great sphere.
    bool intersects_great_sphere(const Vector4& center) const
    {
      return center.dot(position()) >= cos_great_dist;
    }

    // Tells if two given sphere intersects
    bool intersects(const VolSphere& other, float &cos_dist) const
    {
      // Calculates the cossine of the angle between the center of
      // the two spheres.
      cos_dist = this->position().dot(other.position());

      // The following algorithm fails if the sum of the radius of the
      // spheres is greater than or equal 180°. But in this case, both
      // spheres are always touching:
      if((radius + other.radius) >= math::pi) {
	return true;
      }

      // Calculates the cossine of the sum of the radius.
      float touching_radius = std::cos(radius + other.radius);

      // True if distance between centers is smaller or equal
      // the radius touching distance
      return cos_dist >= touching_radius;
    }

    bool intersects(const VolSphere& other) const
    {
      float cos_dist;
      return intersects(other, cos_dist);
    }

    // Tells if a given point is contained in the sphere
    bool contains(const Vector4 &p) const
    {
      return this->position().dot(p) < std::cos(radius);
    }

    float get_radius() const
    {
      return radius;
    }

  private:
    float radius;

    // Great spheres (sphere of radius π) are a special case
    // because they are used extensively by the spheric Octree
    // algorithm, so it makes sense to have the value precomputed.
    // This is the cosine of the sum of the radius of this sphere
    // and the radius of a great sphere (π / 2).
    float cos_great_dist;
};
