#pragma once

#include "matrix4.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace Glome
{
	class Drawable : virtual public Object
	{
	public:
		virtual ~Drawable() {};
		virtual void draw(Camera& c) = 0;
	};
}
