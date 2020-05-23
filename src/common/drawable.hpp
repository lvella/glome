#pragma once

#include "camera.hpp"
#include "object.hpp"

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
