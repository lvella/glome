#pragma once

#include "gl.hpp"
#include "matrix4.hpp"
#include "vector2.hpp"
#include "math.hpp"

#include <array>
#include <bitset>
#include <vector>
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
		glUniform2fv(id, 1, val.data());
	}

	void set(const Vector4 &val) const
	{
		glUniform4fv(id, 1, val.data());
	}

	void set(const Matrix4 &val) const
	{
		val.loadTo(id);
	}

	void set(const QRot& val) const
	{
		val.loadToUniform(id);
	}

private:
	// Lightweight object, should have no other attribute.
	GLint id = -1;
};

class Shader
{
public:
	// Attributes locations, in order:
	enum Attributes: GLint {
		ATTR_POSITION,
		ATTR_COLOR,
		ATTR_TEXCOORD,
		NUM_ATTRIBUTES
	};

	// Attributes names, in order:
	static constexpr char const * attr_names[NUM_ATTRIBUTES] = {
		"position",
		"color",
		"texcoord"
	};

	using SourceVector = std::vector<const char*>;

	Shader() = default;
	Shader(const SourceVector& sources);
	virtual ~Shader();

	virtual void setup_shader(const SourceVector& sources);

	void enable() const;

	Uniform getUniform(const char *name) const;

	GLuint program() const {
		return prog;
	}

protected:
	GLuint prog = 0;
};

class SpaceShader: public Shader
{
public:
	virtual ~SpaceShader() = default;
	virtual void setup_shader(const SourceVector& sources) override;

	void setTransform(const QRot& t) const
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
	static constexpr float Z_FAR = 2.1f;

	static void initialize(float aspect_ratio);
	static const Matrix4& getProjection()
	{
		return proj_mat;
	}

	virtual void setup_shader(const SourceVector& sources) override;

private:
	static Matrix4 proj_mat;
	static bool initialized;
};
