#pragma once

#include <functional>
#include <cassert>
#include <cmath>
#include "object.hpp"

// This class represents a sphere for the
// purposes of collision detection.
template<class Location>
class VolSphere: virtual private Location
{
  public:
    // Build a sphere from center and radius. As usual
    // for the spheric space, the center must be normalized,
    // and the radius, as any distance, must be given in radians.
    // The radius must be given in range [0, PI)
    VolSphere(float radius=M_PI_2):
      radius(radius)
    {
      assert(radius >= 0);
      assert(radius < M_PI);
    }
    
    template<typename Q> friend class VolSphere;

    // Tells if two given sphere intersects
    template <class OtherLoc>
    bool intersects(const VolSphere<OtherLoc>& other)
    {
      // Calculates the cossine of the angle between the center of
      // the two spheres.
      float cos_dist = this->position().dot(other.position());

      // Calculates the cossine of the sum of the radius.
      float touching_radius = std::cos(radius + other.radius);

      // True if distance between centers is smaller or equal
      // the radius touching distance
      bool intersecting = cos_dist >= touching_radius;

      // The prevous algorithm fails if the sum of the radius of the
      // spheres is greater than or equal 180°, in which case
      // radius_touching wraps around. In this case, both spheres
      // are always touching:
      bool always_touching = (radius + other.radius) >= M_PI;

      return always_touching || intersecting;
    }

    // Tells if a given point is contained in the sphere
    bool contains(const Vector4 &p)
    {
      return this->position().dot(p) < std::cos(radius);
    }

    float get_radius()
    {
      return radius;
    }

  private:
    float radius;
};
