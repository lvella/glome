#pragma once

#include "matrix4.hpp"
#include "shader.hpp"
#include "object.hpp"

namespace drawable
{
	class Drawable : virtual public Object
	{
	public:
		Drawable() {};
		virtual ~Drawable() {};
		virtual void draw(const Shader& cam) = 0;
	};
}
