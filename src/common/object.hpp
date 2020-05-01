#pragma once

#include "qrot.hpp"
#include "camera.hpp"

class Object
{
public:
	explicit Object(const QRot& init = QRot::IDENTITY()): _t(init) {}
	virtual ~Object() = 0;

	const QRot &transformation() const { return _t; }
	void set_transformation(const QRot& ref) { _t = ref; }

	const Vector4 position() const {
		return _t.position();
	}

protected:
	QRot _t;
};

