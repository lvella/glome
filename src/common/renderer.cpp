#include "drawable.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"
#include "dustfield.hpp"
#include "renderer.hpp"
#include "vector4.hpp"

#include <algorithm>

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
Renderer::draw(vector<Glome::Drawable*>&& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();
		
		camera.reset(active->transformation());
		camera.pushShader(&shader);

		const QRot inv_trans = active->transformation().inverse();
		const Vector4 cam_pos = inv_trans.position();

		// HDX
		createViewingFustrum(objs, inv_trans);

		vector<std::pair<float, Glome::Drawable*>> sorted;
		sorted.reserve(objs.size());
		for(Glome::Drawable* ptr: objs) {
			float dist = std::acos(cam_pos.dot(ptr->position()))
				- ptr->get_radius();
			sorted.push_back({dist, ptr});
		}
		std::sort(sorted.begin(), sorted.end(), [] (auto& a, auto& b) {
			return a.first > b.first;
		});

		auto sorted_projs = Projectile::cull_sort_from_camera(camera);

		draw_meridians(camera);

		for(auto &pair: sorted) {
			//TODO: not taking into account multiplayer (HDX)
			#ifdef FUSTRUM_CULLING
				if (pair.second->isInView)
					pair.second->draw(camera);
			#else
				pair->second->draw(camera);
				objsInView++;
			#endif
		}

		Projectile::draw_many(sorted_projs, camera);
		DustField::draw(camera);

		MiniMap::draw(active->_x, active->_y, this,
			inv_trans, objs
		);
	}
}

void
Renderer::fill_minimap(const vector<Glome::Drawable*>& objs, Camera &cam)
{
	std::shared_ptr<Glome::Drawable> curr = active->t.lock();

	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
	// Probably so insignificant it is not worth the effort.
	for(auto &obj: objs) {
		if(obj != curr.get())
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


void Renderer::createViewingFustrum(const vector<Glome::Drawable*>& objs, const QRot& cameraTransform) {

	printf("alooo");
	
	#ifdef FUSTRUM_CULLING
		#warning "Fustrum Culling is ON"
		// ============================================= //
		// ============== Fustrum culling ============== //
		// ============================================= //
		//TODO: this should not be here, it is being computed every frame
		//TODO: put it someplace else where it is only done once and then 
		//TODO: save the planes so matrix transformations can be applied 
		//TODO: over them as the ship moves
		// need to find the centers of the 5 circles of the fustrum (the near clipping plane can be ignored)
		// except for the far clipping plane, they are 90Âº from the ship/player
		Vector4 topClippingPlaneCenter{0,-1,0,0};
		Vector4 bottomClippingPlaneCenter{0,+1,0,0};
		Vector4 leftClippingPlaneCenter{+1,0,0,0};
		Vector4 rightClippingPlaneCenter{-1,0,0,0};
		// all these will rotate towards z=-1
		// get the rotation matrices
		// top and bottom rotate by FOV/2
		// FOVx = FOVy*aspectratio
		float yzPlaneRotation = CamShader::FOV_Y/2;
		float xzPlaneRotation = CamShader::FOV_Y*(float(Options::width) / float(Options::height)) / 2;
		// left and right rotate by aspectRatio/2
		QRot topClippingPlaneRotation = yz_qrot(yzPlaneRotation);
		QRot bottomClippingPlaneRotation = yz_qrot(-yzPlaneRotation);
		QRot leftClippingPlaneRotation = xz_qrot(-xzPlaneRotation);
		QRot rightClippingPlaneRotation = xz_qrot(xzPlaneRotation);
		// apply the rotation to the centers of the planes/circles
		// multiply the vectors with the rotation matrices
		topClippingPlaneCenter = topClippingPlaneRotation*topClippingPlaneCenter;
		bottomClippingPlaneCenter = bottomClippingPlaneRotation*bottomClippingPlaneCenter;
		leftClippingPlaneCenter = leftClippingPlaneRotation*leftClippingPlaneCenter;
		rightClippingPlaneCenter = rightClippingPlaneRotation*rightClippingPlaneCenter;
		// for the far clipping plane
		// get the point parallel to the ship in the far clipping plane
		Vector4 S = Vector3(0,0,-CamShader::Z_FAR/2).inverse_stereo_proj();
		Vector4 farClippingPlaneCenter = -((Vector4{0,0,0,1} + S)*0.5).normalized();
		float farClippingPlaneCosRadius = Vector4(0,0,0,1).dot(farClippingPlaneCenter);

		// multiply the planes by the camera
		topClippingPlaneCenter = cameraTransform*topClippingPlaneCenter;
		bottomClippingPlaneCenter = cameraTransform*bottomClippingPlaneCenter;
		leftClippingPlaneCenter = cameraTransform*leftClippingPlaneCenter;
		rightClippingPlaneCenter = cameraTransform*rightClippingPlaneCenter;
		farClippingPlaneCenter = cameraTransform*farClippingPlaneCenter;


		// now that we have all the planes, check if each object is inside the fustrum
		int objsInView = 0;
		// std::cos(math::pi_2) == 0
		for(auto &obj: objs) {
			if(obj->position().dot(topClippingPlaneCenter) 		  >= 0
				&& obj->position().dot(bottomClippingPlaneCenter) >= 0
				&& obj->position().dot(leftClippingPlaneCenter)   >= 0
				&& obj->position().dot(rightClippingPlaneCenter)  >= 0
				&& obj->position().dot(farClippingPlaneCenter) 	  >= farClippingPlaneCosRadius
			) {
				obj->isInView = true;
				objsInView++;
			} else {
				obj->isInView = false;
			}
		}

		// debug: how many objs are in view
		printf("Total objs: %d, objs in view: %d\n", objs.size(), objsInView);
		// debug: try to draw the planes
		// from the centers, get a point on the plane, 
		// glBegin(GL_QUADS);
		// 	glColor3ub(255, 0, 0);
		// 	glVertex4f(auxTop.x-100, auxTop.y-100, auxTop.z-100, auxTop.w);
		// 	glVertex4f(auxBot.x-100, auxBot.y-100, auxBot.z-100, auxBot.w);
		// 	glVertex4f(auxLeft.x-100, auxLeft.y-100, auxLeft.z-100, auxLeft.w);
		// 	glVertex4f(auxRight.x-100, auxRight.y-100, auxRight.z-100, auxRight.w);
		// glEnd();

	#else
		#warning "Fustrum Culling is OFF"
	#endif
}
