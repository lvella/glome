#include <cassert>

#include "camera.hpp"

Camera::Camera()
{
	reset(Matrix4::IDENTITY);
}

void Camera::reset(const Matrix4& invCam)
{
	mat_stack.resize(1);
	mat_stack[0] = invCam;

	shader_stack.resize(0);
}

void Camera::pushMultMat(const Matrix4& t)
{
	assert(!shader_stack.empty());

	mat_stack.push_back(mat_stack.back() * t);
	shader_stack.back()->setTransform(mat_stack.back());
}

void Camera::popMat()
{
	mat_stack.pop_back();
	//shader_stack.back()->setTransform(mat_stack.back());
}

void Camera::pushShader(const SpaceShader *s)
{
	shader_stack.push_back(s);
	s->enable();
	s->setTransform(mat_stack.back());
}

void Camera::popShader()
{
	shader_stack.pop_back();
	if(!shader_stack.empty()) {
		shader_stack.back()->enable();
	}
}

const SpaceShader* Camera::getShader() const
{
	return shader_stack.back();
}
