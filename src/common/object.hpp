#pragma once

#include "matrix4.hpp"
#include "camera.hpp"

class Object
{
public:
	explicit Object(const Matrix4& init = Matrix4::IDENTITY): _t(init) {}
	virtual ~Object() = 0;

	const Matrix4 &transformation() const { return _t; }
	void set_transformation(const Matrix4& ref) { _t = ref; }

	const Vector4 position() const {
		return _t.position();
	}

protected:
	Matrix4 _t;
};

