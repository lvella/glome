#pragma once

#include "controller.hpp"
#include "renderer.hpp"
#include "ship.hpp"
#include "octree.hpp"
#include "runcontext.hpp"

/** Every game mode should derive this class.
 * TODO: Based upon similarities between the game modes, refactor this class to hold what is common.
 */
class World: public RunContext
{
public:
	virtual ~World() {};

	void setup_display() { _render->setup_display(); }

	void draw() override = 0;
	void update() override = 0;

protected:
	Octree::Hypercube collision_tree;

	Controller* _ctrl;
	Renderer* _render;
	std::vector<Ship*> ships;
	std::vector<AiController*> ai_controls;
};

