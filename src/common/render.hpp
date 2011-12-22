#pragma once

#include <deque>
#include <vector>

#include "drawable.hpp"
#include "shader.hpp"
#include "matrix4.hpp"
#include "ship.hpp"

class Render : public Drawable
{
public:
	virtual void draw();
	virtual static void setup_display();
	Render(std::vector<Ship*>* pp);

protected:
	std::vector<Ship*>* players;

	std::deque<Matrix4> cam_hist;
	Vector4 cam_pos;

	static Shader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

