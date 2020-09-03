#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "frustum.hpp"

#include "renderer.hpp"
#include "openvr.h"

class RendererVR : public Renderer
{
public:
	using ObjSet = std::unordered_multimap<
		DrawSpecsBase*,
		std::weak_ptr<Glome::Drawable>
	>;

	RendererVR(const std::vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, /*std::shared_ptr<vr::IVRSystem>*/ vr::IVRSystem* const pHMD);

	void update(float dt);
	void draw(ObjSet& objs);

	std::vector<std::shared_ptr<Glome::Drawable>> draw_objs_in_world(ObjSet& objs);

	void setup_display();
	void fill_minimap(const std::vector<std::shared_ptr<Glome::Drawable>>& objs,
		Camera& cam);

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

	std::shared_ptr<Viewport> left_eye;
	std::shared_ptr<Viewport> right_eye;

	Frustum frustum_at_origin;

	GLuint left_eye_framebuffer;
	GLuint right_eye_framebuffer;

	GLuint left_eye_texture;
	GLuint right_eye_texture;

	// std::shared_ptr<vr::IVRSystem> m_pHMD;
	vr::IVRSystem* m_pHMD;
};
