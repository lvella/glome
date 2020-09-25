#pragma once

#include "qrot.hpp"
#include "camera.hpp"

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
		invalidate_cache();
	}

	void mul_t(const QRot& ref)
	{
		set_t(_t * ref);
	}

	const Vector4 get_world_pos() const;

	virtual const QRot& get_world_t() const;

protected:
	virtual void invalidate_cache();

private:
	QRot _t;
	mutable Vector4 pos;
	mutable bool dirty;
};
