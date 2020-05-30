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

		// HDX
		createViewingFustrum(objs);
		
		camera.reset(active->transformation());
		camera.pushShader(&shader);

		const QRot inv_trans = active->transformation().inverse();
		const Vector4 cam_pos = inv_trans.position();

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


void Renderer::createViewingFustrum(const vector<Glome::Drawable*>& objs) {
	
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
		Vector4 topClippingPlaneCenter{0,-1,0,-1};
		Vector4 bottomClippingPlaneCenter{0,+1,0,-1};
		Vector4 leftClippingPlaneCenter{-1,0,0,-1};
		Vector4 rightClippingPlaneCenter{+1,0,0,-1};
		// all these will rotate towards z=-1
		// get the rotation matrices
		// top and bottom rotate by FOV/2
		float xPlaneRotation = CamShader::FOV_Y/2;
		float yPlaneRotation = (float(Options::width) / float(Options::height)) / 2;
		// left and right rotate by aspectRatio/2
		Matrix4 topClippingPlaneRotation = xz_matrix(xPlaneRotation);
		Matrix4 bottomClippingPlaneRotation = xz_matrix(-xPlaneRotation);
		Matrix4 leftClippingPlaneRotation = yz_matrix(yPlaneRotation);
		Matrix4 rightClippingPlaneRotation = yz_matrix(-yPlaneRotation);
		// apply the rotation to the centers of the planes/circles
		// multiply the vectors with the rotation matrices
		topClippingPlaneCenter = topClippingPlaneRotation*topClippingPlaneCenter;
		bottomClippingPlaneCenter = bottomClippingPlaneRotation*bottomClippingPlaneCenter;
		leftClippingPlaneCenter = leftClippingPlaneRotation*leftClippingPlaneCenter;
		rightClippingPlaneCenter = rightClippingPlaneRotation*rightClippingPlaneCenter;
		// for the far clipping plane
		// get the point parallel to the ship in the far clipping plane
		Vector4 S{0,0,CamShader::Z_FAR,-1};
		// get the correct point by applying the inverse transform
		// of the camera (inverse = transpose because of orthonormality)
		Matrix4 inversePerspective(CamShader::getProjection().transpose());
		Vector4 P = inversePerspective*S;
		// how to get the other point???
		// maybe, get the line from the original point and the anti-perspective point
		// and check the intersection with the circle
		// get the line (direction and way)
		Vector4 line = (P - S).normalized();
		// now calculate the intersection with the sphere
		// this should give two points, one we already have, we want the other one
		// equation for a sphere (from wikipedia)
			// || x - c || ^2 = r^2
			// c -> center point
			// r -> radius
			// x -> points on the sphere
		// equation for a line
			// x = o + d*l
			// d -> distance along line from starting point
			// l -> direction of the line
			// o -> origin of the line
			// x -> points on the line
		// d = -( l.(o-c)) +- sqrt( (l.(o-c))^2 - (||o-c||^2 - r^2) )
		// if the value inside the sqrt() is less than zero, no solution exist ==> should never happen in our case
		// if it is zero, one solution exists ==> also should never happen in our case
		// if it is greater than zero, two solutions exist ==> should always be the case
		// so, calculate the inside of the sqrt() first to get the two solutins, then solver for d, we will have two different ds
		// with the ds, we can calculate the two points that intersect with the sphere by substituing them in the line equation
		Vector4 o_minus_c = S-Vector4(0,0,0,0);
		float temp = line.dot(o_minus_c);
		temp *= temp;
		float temp2 = o_minus_c.length() * o_minus_c.length() - 1;
		float inside_sqrt = temp - temp2;
		printf("inside_sqrt = %f\n", inside_sqrt);
		if(inside_sqrt < 0) {
			printf("ERROR: inside_sqrt < 0\n");
			exit(-1);
		}
		else if(inside_sqrt == 0) {
			printf("ERROR: inside_sqrt == 0\n");
			exit(-1);
		}
		float d1 = -1 * (line.dot(o_minus_c)) + inside_sqrt;
		float d2 = -1 * (line.dot(o_minus_c)) - inside_sqrt;
		Vector4 P2 = S + line*d1;
		if(P2 == P) {
			P2 = S + line*d2;
		}
		// now that I have both points, I want the point in the middle of the two
		Vector4 P_middle{(P.x+P2.x)/2, (P.y+P2.y)/2, (P.z+P2.z)/2, (P.w+P2.w)/2};
		// normalize to get it in the circumference
		P_middle = P_middle.normalized();
		// now get the opposite point in the 4D sphere
		Vector4 P_definite = P_middle * -1;
		// calculate the radius of the far clipping plane
		float farClippingPlaneRadius = acos(P_definite.dot(S));
		// with the radius, I can get the far clipping plane center
		// with the origin at S, move radius in the direction of the ship
		Vector4 direction = S-Vector4(0,0,0,-1);
		direction = direction.normalized();
		Vector4 farClippingPlaneCenter = S + direction*farClippingPlaneRadius;



		// now that we have all the planes, check if each object is inside the fustrum
		// I though it was supposed to be '<' but this way no objects get caught in the fustrum ???
		int objsInView = 0;
		for(auto &obj: objs) {
			if(obj->position().dot(topClippingPlaneCenter) 		  > cos(90)
				&& obj->position().dot(bottomClippingPlaneCenter) > cos(90)
				&& obj->position().dot(leftClippingPlaneCenter)   > cos(90)
				&& obj->position().dot(rightClippingPlaneCenter)  > cos(90)
				// && obj->position().dot(farClippingPlaneCenter) 	  < cos(90)
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
