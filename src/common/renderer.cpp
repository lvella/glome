#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"
#include "dustfield.hpp"

#include "renderer.hpp"

using namespace std;
using namespace Options;

void
Renderer::initialize()
{
	const char *sources[] = {
		"world.vert", "world.frag", "world_fog.frag",
		"no_texture.frag", "fog.frag", NULL
	};

	shader.setup_shader(sources);
}

void
Renderer::setup_display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_NOTEQUAL, 0.0f);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Renderer::Renderer(const vector<Ship*>& pp, Audio::World &audio_world)
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
	}
}

void
Renderer::draw(const vector<Glome::Drawable*>& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();

		camera.reset(active->transformation());
		camera.pushShader(&shader);

		auto sorted_projs = Projectile::cull_sort_from_camera(camera);

	        draw_meridians(camera);

		for(auto &obj: objs) {
			obj->draw(camera);
		}

		Projectile::draw_many(sorted_projs, camera);
		DustField::draw(camera, active->transformation());

		MiniMap::draw(active->_x, active->_y, this, active->t->transformation().transpose(), objs);
	}
}

void
Renderer::fill_minimap(const vector<Glome::Drawable*>& objs, Camera &cam)
{
	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
	// Probably so insignificant it is not worth the effort.
	for(auto &obj: objs) {
		if(obj != active->t)
			obj->minimap_draw(cam);
	}
}

void
Renderer::audio_update()
{
	for(auto &p :players)
	{
		p.Audio::Listener::update(p.transformation());
	}
}

void
Renderer::Viewport::update(float dt)
{
	cam_hist.push_back({dt, cam_offset * QRot(t->transformation().transpose())});

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

	curr_mat = curr_qrot.toMatrix4();
}

CamShader Renderer::shader;
const QRot Renderer::Viewport::cam_offset(QRot(
	yz_matrix(0.2) *
	zw_matrix(-0.015) *
	yw_matrix(-0.01)
));
