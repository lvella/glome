#pragma once

#include <list>
#include <memory>

#include "renderer.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "ship.hpp"
#include "octree.hpp"
#include "runcontext.hpp"
#include "audio_world.hpp"

/** Every game mode should derive this class.
 * TODO: Based upon similarities between the game modes, refactor this class to hold what is common.
 */
class World: public RunContext, public Audio::World
{
public:
	World();

	virtual ~World() = default;

	void setup_display() override
	{
		_render->setup_display();
	}

	void update(float dt) override;
	void draw() override;

protected:
	void add_updatable(std::shared_ptr<Updatable>&& new_obj);

	Octree::Hypercube collision_tree;

	Renderer* _render;
	std::vector<std::weak_ptr<Ship>> ships;
	std::vector<AiController*> ai_controls;

private:

	std::vector<std::shared_ptr<Updatable>> updatables;
	std::vector<std::weak_ptr<Collidable>> collidables;
	std::vector<std::weak_ptr<Glome::Drawable>> drawables;
};

