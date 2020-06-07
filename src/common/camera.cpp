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
}

void Camera::pushMultQRot(const QRot& t)
{
	assert(shader);

	qrot_stack.push_back(qrot_stack.back() * t);
	shader->setTransform(qrot_stack.back());
}

void Camera::popMat()
{
	qrot_stack.pop_back();
	//shader_stack.back()->setTransform(qrot_stack.back());
}

void Camera::setShader(const SpaceShader *s)
{
	shader = s;
	s->enable();
	s->setTransform(qrot_stack.back());
}

const SpaceShader* Camera::getShader() const
{
	return shader;
}
