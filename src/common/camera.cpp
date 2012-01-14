#include <cassert>

#include "camera.hpp"

Camera::Camera()
{
	reset(Matrix4::IDENTITY, NULL);
}

void Camera::setProjection(const Matrix4& p)
{
	PROJ_MAT = p;
}

void Camera::reset(const Matrix4& invCam, const CamShader *s)
{
	stack.resize(1);
	stack[0] = invCam;

	setShader(s);
}

void Camera::pushMult(const Matrix4& t)
{
	assert(shader != NULL);

	stack.push_back(stack.back() * t);
	shader->setTransform(stack.back());
}

void Camera::pop()
{
	stack.pop_back();
	//shader->setTransform(stack.back());
}

void Camera::setShader(const CamShader *s)
{
	shader = s;
	if(s) {
		s->enable();
		s->setProjection(Camera::PROJ_MAT);
		s->setTransform(stack.back());
	}
}

const Shader* Camera::getShader() const
{
	return shader;
}

Matrix4 Camera::PROJ_MAT;
