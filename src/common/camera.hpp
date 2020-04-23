#pragma once

#include <vector>

#include "shader.hpp"
#include "matrix4.hpp"

class Camera
{
public:
	Camera();

	// TODO: make the thing to follow the ship here...
	// TODO: create an "update()" method
	void reset(const Matrix4& invCam);

	void pushMultMat(const Matrix4& t);
	void popMat();

	void pushShader(const SpaceShader *s);
	void popShader();
	const SpaceShader* getShader() const;

	// TODO, FOR PERFORMANCE: Implement a kind of flush: that must be
	// called right before drawing, so to flush the properties to the
	// video card... must spare a few video memory writes.
	//void flush();

	const Matrix4& transformation() const {
		return mat_stack.back();
	}

private:
	std::vector<const SpaceShader *> shader_stack;
	std::vector<Matrix4> mat_stack;
};
