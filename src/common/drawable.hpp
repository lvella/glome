#pragma once

#include "matrix4.hpp"
#include "shader.hpp"
#include "object.hpp"

class WillBeDrawnOnTheFuckingScreenX11Cuzao : virtual public Object
{
public:
	WillBeDrawnOnTheFuckingScreenX11Cuzao() {}

	virtual void draw(const Shader& cam) = 0;

protected:
	Matrix4 _t;
};

