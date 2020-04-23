#pragma once

#include "gl.hpp"
#include "matrix4.hpp"
#include "vector2.hpp"
#include "math.hpp"

#include <map>

class Uniform
{
public:
	Uniform() {}
	explicit Uniform(GLint location): id(location) {}

	void set(bool val) const
	{
		glUniform1i(id, (int)val);
	}

	void set(int val) const
	{
		glUniform1i(id, val);
	}

	void set(float val) const
	{
		glUniform1f(id, val);
	}

	void set(const Vector2 &val) const
	{
		glUniform2fv(id, 1, val.getVertex());
	}

	void set(const Vector4 &val) const
	{
		glUniform4fv(id, 1, val.getVertex());
	}

	void set(const Matrix4 &val) const
	{
		val.loadTo(id);
	}

private:
	// Lightweight object, should have no other attribute.
	GLint id = -1;
};

class Shader
{
public:
	Shader() = default;
	Shader(const char *sources[]);
	virtual ~Shader();

	virtual void setup_shader(const char *sources[]);

	void enable() const {
		glUseProgram(prog);
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

	Uniform getUniform(const char *name) const;

	GLuint program() const {
		return prog;
	}

protected:
	GLuint prog = 0;

	GLint attr_color;
	GLint attr_texcoord;
};

class SpaceShader: public Shader
{
public:
	virtual ~SpaceShader() = default;
	virtual void setup_shader(const char *sources[]) override;

	void setTransform(const Matrix4& t) const
	{
		transform.set(t);
	}

private:
	Uniform transform;
};

class CamShader final: public SpaceShader
{
public:
	static constexpr float FOV_Y = math::pi / 4.0f;
	static constexpr float Z_NEAR = 0.007f;
	static constexpr float Z_FAR = 1.7f;

	static void initialize(float aspect_ratio);
	static const Matrix4& getProjection()
	{
		return proj_mat;
	}

	void setup_shader(const char *sources[]) override;

private:
	static Matrix4 proj_mat;
	static bool initialized;
};
