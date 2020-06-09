#include "sub_object.hpp"

const QRot& TreeNode::get_world_t() const
{
	return get_t();
}

const QRot& SubObject::get_world_t() const
{
	if(auto ptr = parent.lock()) {
		world_t = ptr->get_world_t() * get_t();
	}

	return world_t;
}
