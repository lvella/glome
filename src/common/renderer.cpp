#include "renderer.hpp"

#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"

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

Renderer::Renderer(vector<Ship*>* pp)
{
	assert(pp->size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (pp->size() > 2 ? 2 : 1);
	int w = width / (pp->size() > 1 ? 2 : 1);

	for(int i = 0; i < pp->size(); ++i) {
		players.push_back(Viewport(pp->at(i), (i%2) * w, height - (i/2 + 1) * h, w, h));
	}

	// Set non-changing camera perspective
	camera.setProjection(perspective(FOV, float(w) / float(h), 0.001f, 5.0f));
}

void
Renderer::draw(vector<Glome::Drawable*> *objs)
{
	objects = objs;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i < players.size(); ++i) {
		players[i].enable();

		camera.reset(players[i].newCameraTransform());
		camera.pushShader(&shader);
		draw_meridians(camera);

		for(int j = 0; j < objs->size(); ++j) {
			objs->at(j)->draw(camera);
		}

		Projectile::draw_all(camera);
		players[i].drawMiniMap(this);
	}
}

void
Renderer::fill_minimap(Camera &cam)
{
	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
	// Probably so insignificant it is not worth the effort.
	for(size_t i = 1; i < objects->size(); ++i)
		objects->at(i)->minimap_draw(cam);
}

void
Renderer::Viewport::drawMiniMap(Renderer *r)
{
	MiniMap::draw(_x, _y, r, t->transformation().transpose());
}

CamShader Renderer::shader;
