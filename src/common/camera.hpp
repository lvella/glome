#pragma once

#include <vector>

#include "shader.hpp"
#include "matrix4.hpp"

class Camera
{
public:
	Camera();
	static void setProjection(const Matrix4& p);

	// TODO: make the thing to follow the ship here...
	// TODO: create an "update()" method
	void reset(const Matrix4& invCam, const CamShader *s);

	void pushMult(const Matrix4& t);
	void pop();

	void setShader(const CamShader *s);
	const Shader* getShader() const;

private:
	static Matrix4 PROJ_MAT;

	const CamShader *shader;
	std::vector<Matrix4> stack;
};
