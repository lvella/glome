#pragma once

#include "matrix4.hpp"
#include "shader.hpp"

class Object
{
public:
	Object(const Matrix4& init = Matrix4::IDENTITY): _t(init) {}

	virtual ~Object() = 0;

	const Matrix4 transformation() const { return _t; }

	void set_transformation(const Matrix4& ref) { _t = ref; }

protected:
	Matrix4 _t;
};

