#pragma once

#include <vector>

#include "shader.hpp"
#include "matrix4.hpp"

class Camera
{
public:
	Camera(const QRot& invCam);

	QRot setQRot(const QRot& t);

	void setShader(const SpaceShader *s);
	const SpaceShader* getShader() const;

	const QRot& getBaseTransformation() const
	{
		return base_trans;
	}

private:
	const SpaceShader* shader = nullptr;
	const QRot& base_trans;
};
