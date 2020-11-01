#include "world.hpp"

#include <memory>

#include "audio_world.hpp"
#include "drawable.hpp"
#include "projectile.hpp"
#include "thread_pool.hpp"
#include "utils.hpp"
#include "audio_source.hpp"
#include "profiling.hpp"

World::World():
	threads_sync(globalThreadPool.get_num_threads())
{}

void World::add_unmanaged(const std::shared_ptr<Object>& new_obj)
{
	if(auto ptr = std::dynamic_pointer_cast<Collidable>(new_obj)) {
		collidables.emplace_back(ptr);
	}

	if(auto ptr = std::dynamic_pointer_cast<Glome::Drawable>(new_obj)) {
		drawables.emplace(&ptr->get_draw_specs(), ptr);
	}

	Audio::World::try_add_source(new_obj);

	if(auto ptr = std::dynamic_pointer_cast<SuperObject>(new_obj)) {
		auto subobjs = ptr->get_sub_objects();
		for(auto& wptr: subobjs) {
			if(auto sptr = wptr.lock()) {
				add_unmanaged(sptr);
			}
		}
	}
}

void World::add_updatable(std::shared_ptr<Updatable>&& new_obj)
{
	add_unmanaged(new_obj);
	updatables.emplace_back(std::move(new_obj));
}

bool World::update(float dt)
{
	LatchWaiter waiter;
	{
		static TimeAccumulator& update_ta = globalProfiler.newTimer("Update objects");
		TimeGuard timer(update_ta);

		for(unsigned i = 0; i < ai_controls.size(); ++i) {
			ai_controls[i]->act();
		}

		Adder adder;
		globalThreadPool.parallel_run_and_wait([&] (auto&& add_task) {
			for(size_t i = 0; i < updatables.size(); i += CHUNCK_SIZE) {
				add_task([&, i] {
					const size_t max = std::min(
						i + CHUNCK_SIZE,
						updatables.size()
					);

					for(size_t j = i; j < max; ++j) {
						if(!updatables[j]->update(dt, adder)) {
							updatables[j].reset();
						}
					}
				});
			}
		});

		waiter = globalThreadPool.run_in_all_pool_threads([this] (unsigned i) {
			threads_sync[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		});

		remove_if(updatables, [](auto& elem){
			return !elem;
		});
		adder.add_elems_to_world(*this);

		// Projectile must be updated after ships, because they may have fired.
		Projectile::update_all(dt);
	}

	{
		static TimeAccumulator& octree_ta = globalProfiler.newTimer("Octree collide");
		TimeGuard timer(octree_ta);

		std::vector<std::shared_ptr<Collidable>> sptrs;
		std::vector<Collidable*> ptrs;
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
		static TimeAccumulator& audio_ta = globalProfiler.newTimer(
			"Update renderer with audio");
		TimeGuard timer(audio_ta);

		_render->update(dt);
	}

	return true;
}

void World::draw()
{
	static TimeAccumulator& draw_ta = globalProfiler.newTimer("Draw objects");
	TimeGuard timer(draw_ta);

	for(auto& sync: threads_sync) {
		glWaitSync(sync, 0, GL_TIMEOUT_IGNORED);
		glDeleteSync(sync);
		sync = 0;
	}

	_render->draw(drawables);
}
