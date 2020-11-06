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

GLuint Renderer::VertexArrayID;

const QRot Renderer::cam_offset {
	yz_qrot(0.2) *
	zw_qrot(-0.015) *
	yw_qrot(-0.01)
};

const Frustum Renderer::frustum_at_origin = Frustum::atOrigin();

void Renderer::initialize()
{
	// OpenGL nonchanging settings
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

MultiViewRenderer::MultiViewRenderer(std::vector<std::unique_ptr<Renderer>>&& sub)
{
	assert(sub.size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (sub.size() > 2 ? 2 : 1);
	int w = width / (sub.size() > 1 ? 2 : 1);

	viewports.reserve(sub.size());

	for(int i = 0; i < sub.size(); ++i) {
		viewports.push_back({std::move(sub[i]),
			(i%2) * w, height - (i/2 + 1) * h, w, h});
	}

	Fire::set_width(w);
}

void
MultiViewRenderer::update(float dt)
{
	for(Viewport& v: this->viewports) {
		v.renderer->update(dt);
	}
}

void
MultiViewRenderer::draw(ObjSet& objs)
{
	for(Viewport& v: viewports) {
		glViewport(v._x, v._y, v._w, v._h);
		v.renderer->draw(objs);
	}
}

PlayerScoreRenderer::PlayerScoreRenderer(std::weak_ptr<Ship> player, Audio::World &audio_world):
	Audio::Listener(&audio_world),
	target(player),
	curr_qrot(cam_offset),
	score(gltCreateText(), gltDeleteText)
{
	assert(score);

	set_score(0);

	// gltCreateText() messes with VAO, so we need to reset:
	glBindVertexArray(VertexArrayID);

	cam_hist.push_back({1.0f / 6.0f, cam_offset});
}

void
PlayerScoreRenderer::update(float dt)
{
	// Scale of the score text fades after 1 second:
	score_anim_effect = std::max(0.0f, score_anim_effect - dt);

	QRot new_trans;
	if(auto ptr = target.lock()) {
		new_trans = cam_offset * ptr->get_t().inverse();
		set_score_if_different(ptr->ctrl->get_points());
	} else {
		new_trans = cam_hist.back().t;
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

	Audio::Listener::update(dt);

	next.dt -= dt;
}

void
PlayerScoreRenderer::draw(ObjSet& objs)
{
	// Setup GL state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera camera(transformation());

	draw_objects(camera, objs);

	DustField::draw(camera);

	draw_score();

	MiniMap::draw(this, transformation(), objs);
}

void
PlayerScoreRenderer::fill_minimap(const ObjSet& objs, Camera &cam)
{
	std::shared_ptr<Glome::Drawable> curr = target.lock();

	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
	// Probably so insignificant it is not worth the effort.
	for(auto &e: objs) {
		auto obj = e.second.lock();

		if(obj && obj != curr)
			obj->minimap_draw(cam);
	}
}

void
PlayerScoreRenderer::draw_objects(Camera& camera, ObjSet& objs)
{
	SpecsTracker specs(camera);

	const Vector4 cam_pos = transformation().inverse().position();

	vector<std::pair<float, Glome::Drawable*>> transparent_objs;

	Frustum frustum = transformation().inverse() * frustum_at_origin;

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

		++iter;
	}

	std::sort(transparent_objs.begin(), transparent_objs.end(),
		[] (auto& a, auto& b) {
			return a.first > b.first;
		}
	);

	for(auto &pair: transparent_objs) {
		auto& obj = *pair.second;
		specs.maybe_set(&obj.get_draw_specs());
		obj.draw(camera);
	}
}

void
PlayerScoreRenderer::draw_score()
{
	// Draw score text:
	gltBeginDraw();
	gltColor(1.0f, 0.5f, std::min(1.0f, 0.5f + score_anim_effect), 1.0f);
	gltDrawText2D(score.get(), 15.0f, 15.0f, 3.0f + score_anim_effect);
	gltEndDraw();
	glBindVertexArray(VertexArrayID);
}

const QRot &PlayerScoreRenderer::transformation() const
{
	return curr_qrot;
}

void
PlayerScoreRenderer::set_score(uint64_t points)
{
	char score_text[64];
	snprintf(score_text, sizeof score_text, "Score: %lu", points);

	gltSetText(score.get(), score_text);
	glBindVertexArray(VertexArrayID);

	last_set_score = points;
}

void
PlayerScoreRenderer::set_score_if_different(uint64_t points)
{
	if(points != last_set_score) {
		set_score(points);
		// Animate text at every score change:
		score_anim_effect += 0.7;
	}
}
