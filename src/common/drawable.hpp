#pragma once

#include "matrix4.hpp"
#include "shader.hpp"

class drawable : public object
{
public:
	drawable() {}

	virtual ~drawable() = 0;

	virtual void draw(const Shader& cam) = 0;

protected:
	Matrix4 _t;
};

