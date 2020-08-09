#pragma once

#include "qrot.hpp"
#include "camera.hpp"
#include <memory>

class Object
{
public:
	explicit Object(const QRot& init = QRot::IDENTITY())
	{
		set_t(init);
	}
	virtual ~Object() = 0;

	const QRot& get_t() const
	{
		return _t;
	}

	void set_t(const QRot& ref)
	{
		_t = ref;
		pos = ref.position();
	}

	virtual const QRot& get_world_t() const
	{
		return _t;
	}

	void mul_t(const QRot& ref)
	{
		set_t(_t * ref);
	}

	const Vector4& position() const {
		return pos;
	}

private:
	QRot _t;
	Vector4 pos;
};
