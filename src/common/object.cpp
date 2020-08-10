#include "object.hpp"

Object::~Object()
{}


void Object::set_t(const QRot& ref)
{
	_t = ref;
	dirty = true;
}

const QRot& Object::get_world_t() const
{
	return _t;
}

const Vector4 Object::get_world_pos() const
{
	if(dirty) {
		pos = _t.position();
		dirty = false;
	}
	return pos;
}
