#include <cassert>

#include "camera.hpp"

Camera::Camera()
{
	reset(QRot::IDENTITY());
}

void Camera::reset(const QRot& invCam)
{
	qrot_stack.resize(1);
	qrot_stack[0] = invCam;

	shader_stack.resize(0);
}

void Camera::pushMultQRot(const QRot& t)
{
	assert(!shader_stack.empty());

	qrot_stack.push_back(qrot_stack.back() * t);
	shader_stack.back()->setTransform(qrot_stack.back());
}

void Camera::popMat()
{
	qrot_stack.pop_back();
	//shader_stack.back()->setTransform(qrot_stack.back());
}

void Camera::pushShader(const SpaceShader *s)
{
	shader_stack.push_back(s);
	s->enable();
	s->setTransform(qrot_stack.back());
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
