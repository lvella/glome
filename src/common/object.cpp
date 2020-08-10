#include "object.hpp"

Object::~Object()
{}

const Vector4 Object::get_world_pos() const
{
	if(dirty) {
		pos = get_world_t().position();
		dirty = false;
	}
	return pos;
}

const QRot& Object::get_world_t() const
{
	return _t;
}

void Object::invalidate_cache()
{
	dirty = true;
}
