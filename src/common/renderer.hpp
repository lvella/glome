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

	Renderer();

	void draw(std::vector<Glome::Drawable*> *objs);
	void setup_display();
	void fill_minimap();

	Renderer(std::vector<Ship*>* pp);

protected:
	class Viewport
	{
	public:
		Viewport(Ship* target, int x, int y, int w, int h):
			t(target), _x(x), _y(y), _w(w), _h(h)
		{
			cam_hist.resize(10, Matrix4::IDENTITY);
		}

		void enable()
		{
			glViewport(_x, _y, _w, _h);
		}

		Matrix4 newCameraTransform()
		{
			// Calculate camera position
			const Matrix4 cam_offset(yz_matrix(0.2) * zw_matrix(-0.015) * yw_matrix(-0.01));
			Matrix4 ret;

			ret = cam_offset * cam_hist.front();

			cam_hist.pop_front();
			cam_hist.push_back(t->transformation().transpose());

			return ret;
		}

	private:
		Ship* t;
		std::deque<Matrix4> cam_hist;

		int _x, _y, _w, _h;
	};

	std::vector<Viewport> players;
	std::vector<Glome::Drawable*> *objects;

	Camera camera;

	static CamShader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

