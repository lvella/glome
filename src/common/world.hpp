#pragma once

#include "shader.hpp"
#include "matrix4.hpp"

class World
{
public:
	static void initialize();
	static void load_attr_format();

	virtual ~World();

	/** Prepares display to render this world. */
	virtual void setup_display();
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void fill_minimap() = 0;

protected:
	static Shader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};
