#pragma once

class World
{
public:
	virtual ~World();

	/** Prepares display to render this world. */
	virtual void setup_display();
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void fill_minimap() = 0;
};
