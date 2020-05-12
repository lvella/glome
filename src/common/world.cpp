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
static void try_add(std::vector<std::weak_ptr<T>>& v, const std::shared_ptr<Updatable>& obj)
{
	auto cast_obj = std::dynamic_pointer_cast<T>(obj);
	if(cast_obj) {
		v.emplace_back(cast_obj);
	}
}

template<typename T, typename F>
static void remove_if(std::vector<T>& v, F&& func)
{
	auto curr = v.begin();
	auto last_empty = v.begin();
	auto end = v.end();
	while(curr != end) {
		if(!func(*curr)) {
			if(curr != last_empty) {
				*last_empty = std::move(*curr);
			}
			++last_empty;
		}

		++curr;
	}

	if(last_empty != curr) {
		v.resize(last_empty - v.begin());
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

		remove_if(collidables, [&](auto& wptr) {
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
	objects.reserve(drawables.size());
	sobjects.reserve(drawables.size());

	remove_if(drawables, [&](auto& wptr) {
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
