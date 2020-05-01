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
	void reset(const QRot& invCam);

	void pushMultQRot(const QRot& t);
	void popMat();

	void pushShader(const SpaceShader *s);
	void popShader();
	const SpaceShader* getShader() const;

	// TODO, FOR PERFORMANCE: Implement a kind of flush: that must be
	// called right before drawing, so to flush the properties to the
	// video card... must spare a few video memory writes.
	//void flush();

	const QRot& transformation() const {
		return qrot_stack.back();
	}

private:
	std::vector<const SpaceShader *> shader_stack;
	std::vector<QRot> qrot_stack;
};
