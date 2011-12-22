#pragma once

#include "controller.hpp"
#include "render.hpp"
#include "ship.hpp"

/** Every game mode should derive this class. */
class World
{
public:
	World() {};
	virtual ~World() {};

	static void initialize();
	void setup_display() { _render->setup_display(); }

	virtual void draw() = 0;
	virtual void update() = 0;

protected:
	Controller* _ctrl;
	Render* _render;
	std::vector<Ship*> players;
};

