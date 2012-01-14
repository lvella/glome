#pragma once

#include <deque>
#include <vector>

#include "camera.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "matrix4.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "spaghetti.hpp"
#include "randomcube.hpp"

class Render
{
public:
	static void initialize();

	Render();

	void draw();
	void setup_display();
	void fill_minimap();

	Render(std::vector<Ship*>* pp);

protected:
	std::vector<Ship*>* players;
	Spaghetti spg;
	RandomCube cube;

	Camera camera;
	std::deque<Matrix4> cam_hist;

	static CamShader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

