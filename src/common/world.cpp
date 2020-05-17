#include "world.hpp"

#include <memory>

#include "drawable.hpp"
#include "projectile.hpp"
#include "thread_pool.hpp"
#include "utils.hpp"

using namespace std::chrono_literals;

World::World():
	profiler(5000ms)
{}

template<typename T>
static void try_add(std::vector<std::weak_ptr<T>>& v, const std::shared_ptr<Updatable>& obj)
{
	auto cast_obj = std::dynamic_pointer_cast<T>(obj);
	if(cast_obj) {
		v.emplace_back(cast_obj);
	}
}

void World::add_updatable(std::shared_ptr<Updatable> new_obj)
{
	try_add(collidables, new_obj);
	try_add(drawables, new_obj);

	updatables.emplace_back(std::move(new_obj));
}

void World::update(float dt)
{
	profiler.maybe_print();

	{
		constexpr size_t chunk_size = 400;

		static TimeAccumulator& update_ta = profiler.newTimer("Update objects");
		TimeGuard timer(update_ta);

		parallel_run_and_wait(globalThreadPool, [&] (auto&& add_task) {
			for(size_t i = 0; i < updatables.size(); i += chunk_size) {
				add_task([&, i] {
					const size_t max = std::min(
						i + chunk_size,
						updatables.size()
					);

					for(size_t j = i; j < max; ++j) {
						if(!updatables[j]->update(dt, *this)) {
							updatables[j].reset();
						}
					}
				});
			}
		});
		remove_if(updatables, [](auto& elem){
			return !elem;
		});

		for(unsigned i = 0; i < ai_controls.size(); ++i) {
			ai_controls[i]->act();
		}
		Projectile::update_all(dt);
	}

	{
		static TimeAccumulator& octree_ta = profiler.newTimer("Octree collide");
		TimeGuard timer(octree_ta);

		std::vector<std::shared_ptr<VolSphere>> sptrs;
		std::vector<VolSphere*> ptrs;
		sptrs.reserve(collidables.size());
		ptrs.reserve(collidables.size());

		clean_and_for_each_valid(collidables, [&](auto&& ptr) {
			ptrs.push_back(ptr.get());
			sptrs.push_back(std::move(ptr));
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
	objects.reserve(drawables.size());
	sobjects.reserve(drawables.size());

	clean_and_for_each_valid(drawables, [&](auto&& ptr) {
		objects.push_back(ptr.get());
		sobjects.push_back(std::move(ptr));
	});

	_render->draw(std::move(objects));
}
