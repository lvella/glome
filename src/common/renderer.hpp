#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "camera.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "ship.hpp"

class Renderer
{
public:
	static void initialize();

	Renderer(const std::vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world);

	void update(float dt);
	void draw(std::vector<Glome::Drawable*>&& objs);
	void setup_display();
	void fill_minimap(const std::vector<Glome::Drawable*>& objs, Camera& cam);

	void audio_update();

protected:
	struct Viewport: public Audio::Listener
	{
		Viewport(std::weak_ptr<Ship> target,
			int x, int y, int w, int h,
			Audio::World &audio_world
		):
			Audio::Listener(&audio_world),
			t(target), _x(x), _y(y), _w(w), _h(h)
		{
			curr_qrot = cam_offset;
			//curr_qrot = QRot(xw_matrix(2.0) * xy_matrix(-.9) * yz_matrix(1.));
			cam_hist.push_back({1.0f / 6.0f, cam_offset});
		}

		void enable()
		{
			glViewport(_x, _y, _w, _h);
		}

		virtual const QRot &transformation() const override
		{
			return curr_qrot;
		}

		void update(float dt);

		struct PathPoint {
			float dt;
			QRot t;
		};

		std::weak_ptr<Ship> t;
		QRot curr_qrot;
		std::deque<PathPoint> cam_hist;

		int _x, _y, _w, _h;

		// Camera position relative to target...
		static const QRot cam_offset;
	};

	std::vector<Viewport> players;

	std::vector<Viewport>::iterator active;
	Camera camera;

	static CamShader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};
