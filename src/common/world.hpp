#pragma once

#include <list>
#include <memory>
#include <unordered_map>

#include "renderer.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "ship.hpp"
#include "octree.hpp"
#include "runcontext.hpp"
#include "audio_world.hpp"

/** Every game mode should derive this class.
 * TODO: Based upon similarities between the game modes,
 * refactor this class to hold what is common.
 */
class World: public RunContext, public Audio::World
{
public:
	World();

	virtual ~World() = default;

	bool update(float dt) override;
	void draw() override;

protected:
	constexpr static size_t CHUNCK_SIZE = 200;
	class Adder: public UpdatableAdder
	{
	public:
		void add_elems_to_world(World& e)
		{
			for(auto& sptr: new_elems) {
				e.add_updatable(std::move(sptr));
			}
		}
	};

	void add_updatable(std::shared_ptr<Updatable>&& new_obj);
	void add_unmanaged(const std::shared_ptr<Object>& new_obj);

	Octree::Hypercube collision_tree;

	std::unique_ptr<Renderer> _render;
	std::vector<std::weak_ptr<Ship>> ships;
	std::vector<std::shared_ptr<AiController>> ai_controls;

private:
	virtual bool is_alive() = 0;

	std::vector<GLsync> threads_sync;

	std::vector<std::shared_ptr<Updatable>> updatables;
	std::vector<std::weak_ptr<Collidable>> collidables;
	std::unordered_multimap<DrawSpecsBase*, std::weak_ptr<Glome::Drawable>> drawables;
};
