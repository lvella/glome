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

	void mul_t(const QRot& ref)
	{
		set_t(_t * ref);
	}

	virtual void set_t(const QRot& ref);
	virtual const QRot& get_world_t() const;
	virtual const Vector4 get_world_pos() const;

private:
	QRot _t;
	mutable Vector4 pos;
	mutable bool dirty = true;
};
