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

class Renderer
{
public:
	static void initialize();

	Renderer(const std::vector<Ship*>& pp);

	void draw(const std::vector<Glome::Drawable*> &objs);
	void setup_display();
	void fill_minimap(const std::vector<Glome::Drawable*> &objs, Camera &cam);

protected:
	struct Viewport
	{
		Viewport(Ship* target, int x, int y, int w, int h):
			t(target), _x(x), _y(y), _w(w), _h(h)
		{
			cam_hist.resize(10, Matrix4::IDENTITY);
		}

		void enable()
		{
			glViewport(_x, _y, _w, _h);
		}

		inline Matrix4 newCameraTransform();

		Ship* t;
		std::deque<Matrix4> cam_hist;

		int _x, _y, _w, _h;
	};

	std::vector<Viewport> players;

	std::vector<Viewport>::iterator active;
	Camera camera;

	static CamShader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

