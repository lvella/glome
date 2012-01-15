#pragma once

#include "controller.hpp"
#include "renderer.hpp"
#include "ship.hpp"

/** Every game mode should derive this class. */
class World
{
public:
	virtual ~World() {};

	static void initialize();
	void setup_display() { _render->setup_display(); }

	virtual void draw() = 0;
	virtual void update() = 0;

protected:
	Controller* _ctrl;
	Renderer* _render;
	std::vector<Ship*> players;
};

