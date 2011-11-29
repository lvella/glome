#pragma once

#include "gl.hpp"
#include "matrix4.hpp"
#include <map>

class Shader
{
public:
	Shader();
	Shader(const char *sources[]);
	~Shader();

	void setup_shader(const char *sources[]);

	void enable() const {
		glUseProgram(prog);
	}

	void setTransform(const Matrix4& t) const {
    t.loadTo(uniform_transform);
	}

	GLint posAttr() const {
		return 0;
	}

	GLint colorAttr() const {
		return attr_color;
	}

	GLint texcoordAttr() const {
		return attr_texcoord;
	}

	GLuint program() const {
		return prog;
	}

private:


	GLuint prog;

	GLint uniform_transform;

	GLint attr_color;
	GLint attr_texcoord;
};
