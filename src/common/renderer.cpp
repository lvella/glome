#include <future>
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"

#include "renderer.hpp"

using namespace std;
using namespace Options;

void
Renderer::initialize()
{
	const char *sources[] = { "world.vert", "world.frag", "no_texture.frag", "fog.frag", NULL };

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

	// Things that are not in OpenGL ES:
	//TODO: use #ifdefs
	//TODO: load anti-alias settings from configuration
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLineWidth(1.5f);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
}

Renderer::Renderer(const vector<Ship*>& pp)
{
	assert(pp.size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (pp.size() > 2 ? 2 : 1);
	int w = width / (pp.size() > 1 ? 2 : 1);

	for(int i = 0; i < pp.size(); ++i) {
		players.emplace_back(pp[i], (i%2) * w, height - (i/2 + 1) * h, w, h);
	}

	// Set non-changing camera perspective
	camera.setProjection(perspective(FOV, float(w) / float(h), 0.001f, 5.0f));
	Fire::set_width(w);
}

void
Renderer::draw(const vector<Glome::Drawable*>& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();

		camera.reset(active->newCameraTransform());
		camera.pushShader(&shader);

        auto sorted_projs = std::async(&Projectile::cull_sort_from_camera, camera);

        draw_meridians(camera);

		for(auto &obj: objs) {
			obj->draw(camera);
		}

		Projectile::draw_many(sorted_projs.get(), camera);

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

inline Matrix4
Renderer::Viewport::newCameraTransform()
{
	// Calculate camera position
	#ifdef STATS_TUNING
	const float scale = t->get_scale();
	#else
	const float scale = 1.0f;
	#endif
	const Matrix4 cam_offset(
		yz_matrix(0.2) *
		zw_matrix(-0.015 * scale) *
		yw_matrix(-0.01 * scale)
	);

	Matrix4 ret;

	ret = cam_offset * cam_hist.front();

	cam_hist.pop_front();
	cam_hist.push_back(t->transformation().transpose());

	return ret;
}

CamShader Renderer::shader;
