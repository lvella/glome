#pragma once

#include <vector>

#include "shader.hpp"
#include "matrix4.hpp"

class Camera
{
public:
	Camera();
	static void setProjection(const Matrix4& p);
	static const Matrix4& getProjection();

	// TODO: make the thing to follow the ship here...
	// TODO: create an "update()" method
	void reset(const Matrix4& invCam);

	void pushMultMat(const Matrix4& t);
	void popMat();

	void pushShader(const CamShader *s);
	void popShader();
	const Shader* getShader() const;

	// TODO, FOR PERFORMANCE: Implement a kind of flush: that must be
	// called right before drawing, so to flush the properties to the
	// video card... must spare a few video memory writes.
	//void flush();

	const Matrix4& transformation() const {
		return mat_stack.back();
    }

private:
	static Matrix4 PROJ_MAT;

	std::vector<const CamShader *> shader_stack;
	std::vector<Matrix4> mat_stack;
};
