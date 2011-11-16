#pragma once

class World
{
public:
	static void initialize();

	virtual ~World();

	/** Prepares display to render this world. */
	virtual void setup_display();
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void fill_minimap() = 0;

protected:
	static GLuint program;
};
