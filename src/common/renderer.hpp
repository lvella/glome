#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>
#include <list>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "frustum.hpp"
#include "gltext.hpp"

class Renderer
{
public:
	using ObjSet = std::unordered_multimap<
		DrawSpecsBase*,
		std::weak_ptr<Glome::Drawable>
	>;

	static void initialize();

	Renderer(const std::vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world);
	virtual ~Renderer() = default;

	virtual void update(float dt);
	virtual void draw(ObjSet& objs);

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
			t(target), _x(x), _y(y), _w(w), _h(h),
			score(gltCreateText())
		{
			assert(score);
			set_score(0);

			// gltCreateText() messes with VAO, so we need to reset:
			glBindVertexArray(VertexArrayID);

			curr_qrot = cam_offset;
			cam_hist.push_back({1.0f / 6.0f, cam_offset});
		}

		~Viewport()
		{
			gltDeleteText(score);
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

		GLTtext *score = nullptr;

		// Camera position relative to target...
		static const QRot cam_offset;

	private:
		void set_score(uint64_t points);
	};

	std::list<Viewport> players;

	std::list<Viewport>::iterator active;

	Frustum frustum_at_origin;

	static GLuint VertexArrayID;
};
