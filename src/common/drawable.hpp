#pragma once

#include "matrix4.hpp"
#include "camera.hpp"
#include "object.hpp"

// TODO: either remove this namespace, or add it to everything else
namespace Glome
{
	class Drawable : virtual public Object
	{
	public:
		virtual ~Drawable() {};
		virtual void draw(Camera& c) = 0;
		virtual void minimap_draw(Camera& c);
	};
}
