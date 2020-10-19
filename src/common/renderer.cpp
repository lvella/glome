#include "renderer.hpp"

#include <algorithm>
#include <memory>
#include <cstdio>

#include "drawable.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"
#include "dustfield.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "vector4.hpp"

using namespace std;
using namespace Options;

namespace {

class SpecsTracker
{
public:
	SpecsTracker(Camera& camera):
		c(camera)
	{}

	~SpecsTracker()
	{
		shutdown();
	}

	void maybe_set(DrawSpecsBase* s)
	{
		if(s != active) {
			shutdown();
			active = s;
			if(s) s->setup_draw_state(c);
		}
	}

private:
	void shutdown()
	{
		if(active) {
			active->shutdown_draw_state(c);
		}
	}

	Camera& c;
	DrawSpecsBase *active = nullptr;
};

}

void Renderer::initialize()
{	// OpenGL nonchanging settings
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glEnableVertexAttribArray(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);

	glLineWidth(1.5f);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(std::numeric_limits<uint16_t>::max());
}

void
Renderer::setup_display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Renderer::Renderer(const vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world)
{
	assert(pp.size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (pp.size() > 2 ? 2 : 1);
	int w = width / (pp.size() > 1 ? 2 : 1);

	for(int i = 0; i < pp.size(); ++i) {
		players.emplace_back(pp[i], (i%2) * w, height - (i/2 + 1) * h, w, h, audio_world);
	}

	Fire::set_width(w);

	Frustum::initializeAtOrigin(frustum_at_origin);

}

void
Renderer::update(float dt)
{
	for(Viewport& v: players) {
		v.update(dt);
		v.Audio::Listener::update(dt, v.transformation());
	}
}

void
Renderer::draw(ObjSet& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();

		auto drawn_objs = draw_objs_in_world(objs);

		active->draw_score();

		MiniMap::draw(active->_x, active->_y, this,
			active->transformation(), drawn_objs
		);
	}
}

vector<std::shared_ptr<Glome::Drawable>>
Renderer::draw_objs_in_world(ObjSet& objs)
{
	Camera camera(active->transformation());
	SpecsTracker specs(camera);

	const Vector4 cam_pos = active->transformation().inverse().position();

	vector<std::shared_ptr<Glome::Drawable>> remaining_objs;
	vector<std::pair<float, Glome::Drawable*>> transparent_objs;

	Frustum frustum = active->transformation().inverse() * frustum_at_origin;

	for(auto iter = objs.begin(); iter != objs.end();) {
		auto ptr = iter->second.lock();
		if(!ptr) {
			iter = objs.erase(iter);
			continue;
		}

		if(frustum.isIn(*ptr)) {
			if(ptr->is_transparent()) {
				float dist = std::acos(cam_pos.dot(ptr->get_world_pos()))
					- ptr->get_radius();
				assert(!std::isnan(dist));
				transparent_objs.push_back({dist, ptr.get()});
			} else {
				specs.maybe_set(iter->first);
				ptr->draw(camera);
			}
		}

		remaining_objs.emplace_back(std::move(ptr));
		++iter;
	}

	std::sort(transparent_objs.begin(), transparent_objs.end(),
		[] (auto& a, auto& b) {
			return a.first > b.first;
		}
	);

	auto sorted_projs = Projectile::cull_sort_from_camera(camera);
	Projectile::draw_many(sorted_projs, camera);

	for(auto &pair: transparent_objs) {
		auto& obj = *pair.second;
		specs.maybe_set(&obj.get_draw_specs());
		obj.draw(camera);
	}

	DustField::draw(camera);

	return remaining_objs;
}

void
Renderer::fill_minimap(const vector<std::shared_ptr<Glome::Drawable>>& objs, Camera &cam)
{
	std::shared_ptr<Glome::Drawable> curr = active->t.lock();

	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
	// Probably so insignificant it is not worth the effort.
	for(auto &obj: objs) {
		if(obj != curr)
			obj->minimap_draw(cam);
	}
}

void
Renderer::Viewport::draw_score()
{
	// Draw score text:
	gltBeginDraw();
	gltColor(1.0f, 0.5f, std::min(1.0f, 0.5f + score_anim_effect), 1.0f);
	gltDrawText2D(score, 15.0f, 15.0f, 3.0f + score_anim_effect);
	gltEndDraw();
	glBindVertexArray(VertexArrayID);
}

void
Renderer::Viewport::set_score(uint64_t points)
{
	char score_text[64];
	snprintf(score_text, sizeof score_text, "Score: %lu", points);

	gltSetText(score, score_text);
	glBindVertexArray(VertexArrayID);

	last_set_score = points;
}

void
Renderer::Viewport::set_score_if_different(uint64_t points)
{
	if(points != last_set_score) {
		set_score(points);
		// Animate text at every score change:
		score_anim_effect += 0.7;
	}
}

void
Renderer::Viewport::update(float dt)
{
	// Scale of the score text fades after 1 second:
	score_anim_effect = std::max(0.0f, score_anim_effect - dt);

	QRot new_trans;
	if(auto ptr = t.lock()) {
		new_trans = cam_offset * ptr->get_t().inverse();
		set_score_if_different(ptr->ctrl->get_points());
	} else {
		new_trans = cam_hist.front().t;
	}

	cam_hist.push_back({dt, new_trans});

	while(cam_hist.front().dt <= dt) {
		dt -= cam_hist.front().dt;
		curr_qrot = cam_hist.front().t;

		cam_hist.pop_front();
		assert(!cam_hist.empty());
	}

	PathPoint& next = cam_hist.front();
	const float slerp_factor = dt / next.dt; // range [0, 1]

	curr_qrot = nlerp(curr_qrot, next.t, slerp_factor);
	next.dt -= dt;
}

const QRot Renderer::Viewport::cam_offset(
	yz_qrot(0.2) *
	zw_qrot(-0.015) *
	yw_qrot(-0.01)
);

GLuint Renderer::VertexArrayID;
