#pragma once

#include <deque>
#include <vector>

#include "drawable.hpp"
#include "shader.hpp"
#include "matrix4.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "spaghetti.hpp"

class Render
{
public:
	static void initialize();

	virtual void draw();
	virtual void setup_display();
	virtual void fill_minimap();

	Render(std::vector<Ship*>* pp);

protected:
	std::vector<Ship*>* players;
	Spaghetti spg;

	std::deque<Matrix4> cam_hist;
	Vector4 cam_pos;

	static Shader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

