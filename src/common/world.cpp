#include "world.hpp"

#include <memory>

#include "drawable.hpp"
#include "projectile.hpp"
#include "thread_pool.hpp"

using namespace std::chrono_literals;

World::World():
	profiler(5000ms)
{}

template<typename T>
static void try_add(std::list<std::weak_ptr<T>>& list, const std::shared_ptr<Updatable>& obj)
{
	auto cast_obj = std::dynamic_pointer_cast<T>(obj);
	if(cast_obj) {
		list.emplace_back(cast_obj);
	}
}

void World::add_updatable(std::shared_ptr<Updatable> new_obj)
{
	try_add(collidable_list, new_obj);
	try_add(drawable_list, new_obj);

	updatable_list.emplace_back(std::move(new_obj));
}

void World::update(float dt)
{
	profiler.maybe_print();

	{
		constexpr size_t chunk_size = 400;

		static TimeAccumulator& update_ta = profiler.newTimer("Update objects");
		TimeGuard timer(update_ta);

		// TODO: restore parallel update
		updatable_list.remove_if([dt, this](auto& elem){
			return !elem->update(dt, *this);
		});

		for(unsigned i = 0; i < ai_controls.size(); ++i)
			ai_controls[i]->act();
		Projectile::update_all(dt);
	}

	{
		static TimeAccumulator& octree_ta = profiler.newTimer("Octree collide");
		TimeGuard timer(octree_ta);

		std::vector<std::shared_ptr<VolSphere>> sptrs;
		std::vector<VolSphere*> ptrs;
		sptrs.reserve(collidable_list.size());
		ptrs.reserve(collidable_list.size());

		collidable_list.remove_if([&](auto& wptr) {
			auto obj = wptr.lock();
			if(!obj) {
				return true;
			}

			ptrs.push_back(obj.get());
			sptrs.push_back(std::move(obj));
			return false;
		});

		collision_tree.collide(
			Projectile::get_collision_volumes(),
			std::move(ptrs)
		);
	}

	{
		static TimeAccumulator& audio_ta = profiler.newTimer("Update audio");
		TimeGuard timer(audio_ta);
		_render->audio_update();
	}
	_render->update(dt);
}

void World::draw()
{
	static TimeAccumulator& draw_ta = profiler.newTimer("Draw objects");
	TimeGuard timer(draw_ta);

	std::vector<Glome::Drawable*> objects;
	std::vector<std::shared_ptr<Glome::Drawable>> sobjects;
	objects.reserve(drawable_list.size());
	sobjects.reserve(drawable_list.size());

	drawable_list.remove_if([&](auto& wptr) {
		auto obj = wptr.lock();
		if(!obj) {
			return true;
		}

		objects.push_back(obj.get());
		sobjects.push_back(std::move(obj));
		return false;
	});

	_render->draw(std::move(objects));
}
