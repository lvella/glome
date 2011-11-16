#pragma once

#include "matrix4.hpp"

class World
{
public:
	static void initialize();
	static void draw_primitives(GLuint vbo, GLuint ibo, size_t len, const Matrix4& t);

	virtual ~World();

	/** Prepares display to render this world. */
	virtual void setup_display();
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void fill_minimap() = 0;

protected:
	static GLuint shader_program;
	static GLint shader_attr_position;
        static GLint shader_uniform_modelview;
};
