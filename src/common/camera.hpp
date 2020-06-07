#pragma once

#include <vector>

#include "shader.hpp"
#include "matrix4.hpp"

class Camera
{
public:
	Camera();

	void reset(const QRot& invCam);

	void pushMultQRot(const QRot& t);
	void popMat();

	void setShader(const SpaceShader *s);
	const SpaceShader* getShader() const;

	const QRot& transformation() const {
		return qrot_stack.back();
	}

private:
	const SpaceShader* shader = nullptr;
	std::vector<QRot> qrot_stack;
};
