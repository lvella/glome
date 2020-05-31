#include "drawable.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"
#include "dustfield.hpp"

#include "renderer.hpp"
#include <algorithm>
#include <memory>

using namespace std;
using namespace Options;

void
Renderer::initialize()
{
	shader.setup_shader({
		"world/world.vert", "world/modelview.vert", "common/quaternion.vert",
		"world/world.frag", "world/world_fog.frag",
		"common/no_texture.frag", "world/fog.frag"
	});
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
Renderer::draw(vector<std::shared_ptr<Glome::Drawable>>&& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();

		camera.reset(active->transformation());
		camera.pushShader(&shader);

		const QRot inv_trans = active->transformation().inverse();
		const Vector4 cam_pos = inv_trans.position();

		vector<std::pair<float, Glome::Drawable*>> sorted;
		sorted.reserve(objs.size());
		for(auto& ptr: objs) {
			float dist = std::acos(cam_pos.dot(ptr->position()))
				- ptr->get_radius();
			assert(!std::isnan(dist));
			sorted.push_back({dist, ptr.get()});
		}

		std::sort(sorted.begin(), sorted.end(), [] (auto& a, auto& b) {
			return a.first > b.first;
		});

		auto sorted_projs = Projectile::cull_sort_from_camera(camera);

		draw_meridians(camera);

		for(auto &pair: sorted) {
			pair.second->draw(camera);
		}

		Projectile::draw_many(sorted_projs, camera);
		DustField::draw(camera);

		MiniMap::draw(active->_x, active->_y, this,
			inv_trans, objs
		);
	}
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
Renderer::Viewport::update(float dt)
{
	QRot new_trans;
	if(auto ptr = t.lock()) {
		new_trans = cam_offset * ptr->get_t().inverse();
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

CamShader Renderer::shader;
const QRot Renderer::Viewport::cam_offset(
	yz_qrot(0.2) *
	zw_qrot(-0.015) *
	yw_qrot(-0.01)
);
