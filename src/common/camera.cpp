#include <cassert>

#include "camera.hpp"

Camera::Camera(const QRot& invCam):
	base_trans(invCam)
{}

QRot Camera::setQRot(const QRot& t)
{
	assert(shader);

	const QRot trans = base_trans * t;
	shader->setTransform(trans);

	return trans;
}

void Camera::setShader(const SpaceShader *s)
{
	shader = s;
	s->enable();
}

const SpaceShader* Camera::getShader() const
{
	return shader;
}
