#pragma once

#include <deque>
#include <vector>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "camera.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "matrix4.hpp"
#include "math.hpp"
#include "ship.hpp"

class Renderer
{
public:
	static void initialize();

	Renderer(const std::vector<Ship*>& pp, Audio::World &audio_world);

	void update(float dt);
	void draw(const std::vector<Glome::Drawable*> &objs);
	void setup_display();
	void fill_minimap(const std::vector<Glome::Drawable*> &objs, Camera &cam);

	void audio_update();

protected:
	struct Viewport: public Audio::Listener
	{
		Viewport(Ship* target, int x, int y, int w, int h, Audio::World &audio_world):
			Audio::Listener(&audio_world),
			t(target), _x(x), _y(y), _w(w), _h(h)
		{
			cam_hist.push_back({1.0f / 6.0f, cam_offset});
		}

		void enable()
		{
			glViewport(_x, _y, _w, _h);
		}

		virtual const Matrix4 &transformation() const override
		{
			assert(!cam_hist.empty());
			return cam_hist.front().t;
		}

		void update(float dt);

		struct PathPoint {
			float dt;
			Matrix4 t;
		};

		Ship* t;
		std::deque<PathPoint> cam_hist;

		int _x, _y, _w, _h;

		// Camera position relative to target...
		static const Matrix4 cam_offset;
	};

	std::vector<Viewport> players;

	std::vector<Viewport>::iterator active;
	Camera camera;

	static CamShader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};
