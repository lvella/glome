#pragma once

#include "controller.hpp"
#include "renderer.hpp"
#include "ship.hpp"
#include "runcontext.hpp"

/** Every game mode should derive this class.
 * TODO: Based upon similarities between the game modes, refactor this class to hold what is common.
 */
class World: public RunContext
{
public:
	virtual ~World() {};

	void setup_display() { _render->setup_display(); }

	virtual void draw() = 0;
	virtual void update() = 0;

protected:
	Controller* _ctrl;
	Renderer* _render;
	std::vector<Ship*> ships;
	std::vector<AiController*> ai_controls;
};

