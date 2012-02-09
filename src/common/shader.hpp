#pragma once

#include "gl.hpp"
#include "matrix4.hpp"
#include "vector2.hpp"
#include <map>

class Uniform
{
public:
	Uniform() {}
	Uniform(GLint location): id(location) {}

	void set(float val)
	{
		glUniform1f(id, val);
	}

	void set(const Vector2 &val)
	{
		glUniform2fv(id, 1, val.getVertex());
	}

	void set(const Vector4 &val)
	{
		glUniform4fv(id, 1, val.getVertex());
	}

	void set(const Matrix4 &val)
	{
		val.loadTo(id);
	}

private:
	// Lightweight object, should have no other attribute.
	GLint id;
};

class Shader
{
public:
	Shader();
	Shader(const char *sources[]);
	virtual ~Shader();

	virtual void setup_shader(const char *sources[]);

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

	Uniform getUniform(const char *name) const {
		return glGetUniformLocation(prog, name);
	}

	GLuint program() const {
		return prog;
	}

protected:
	GLuint prog;

	GLint uniform_transform;

	GLint attr_color;
	GLint attr_texcoord;
};

class CamShader: public Shader
{
public:
	virtual void setup_shader(const char *sources[]);
	void setProjection(const Matrix4& proj) const;

protected:
	GLint uniform_projection;
};
