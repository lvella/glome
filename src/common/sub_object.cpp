#include "sub_object.hpp"

#include "utils.hpp"

const QRot& SubObject::get_world_t() const
{
	if(dirty) {
		if(auto ptr = parent.lock()) {
			world_t = ptr->get_world_t() * get_t();
		}
	}

	return world_t;
}

void SubObject::invalidate_cache()
{
	Object::invalidate_cache();
	dirty = true;
}


const std::vector<std::weak_ptr<SubObject>>& SuperObject::get_sub_objects()
{
	assert(!initialized);
	initialized = true;

	create_sub_objects(objs);
	return objs;
}

void SuperObject::invalidate_cache()
{
	Object::invalidate_cache();

	clean_and_for_each_valid(objs, [] (std::shared_ptr<SubObject>&& ptr){
		ptr->invalidate_cache();
	});
}
