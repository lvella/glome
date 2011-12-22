#pragma once

#include "matrix4.hpp"
#include "shader.hpp"

class Drawable : virtual public Object
{
public:
	Drawable() {}

	virtual ~Drawable() = 0;

	virtual void draw(const Shader& cam) = 0;

protected:
	Matrix4 _t;
};

