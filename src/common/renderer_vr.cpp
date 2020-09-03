#include "renderer_vr.hpp"

#include <algorithm>
#include <memory>

#include "drawable.hpp"
#include "options.hpp"
#include "meridian.hpp"
#include "minimap.hpp"
#include "projectile.hpp"
#include "fire.hpp"
#include "dustfield.hpp"
#include "camera.hpp"
#include "vector4.hpp"
#include "data_file.hpp"

#include <algorithm>
#include <memory>
#include <iomanip>
#include <stdlib.h>

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

void
RendererVR::setup_display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RendererVR::RendererVR(const vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, /*std::shared_ptr<vr::IVRSystem>*/ vr::IVRSystem* const pHMD) :
	Renderer(pp, audio_world)
{

	m_pHMD = pHMD;

    vr::EVRInitError eError = vr::VRInitError_None;
    
    if ( !vr::VRCompositor() )
    {
        std::cout << "Failed to init VR Compositor: %s" << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        exit(-1);
    }

	// for VR, we have two viewports, one for the left eye and another for the right eye 
	int h = height / 2;
	int w = width / 2;

	left_eye = std::make_shared<Viewport>(pp[0], 0, height - h, w, h, audio_world);
	left_eye = std::make_shared<Viewport>(pp[0], w, height - 1.5f * h, w, h, audio_world);

	left_eye_framebuffer = 0;
	right_eye_framebuffer = 1;

	glGenFramebuffers(1, &left_eye_framebuffer);
	glGenFramebuffers(1, &right_eye_framebuffer);

	Fire::set_width(w);

	Frustum::initializeAtOrigin(frustum_at_origin);
}

void
RendererVR::update(float dt)
{
	left_eye->update(dt);
	left_eye->Audio::Listener::update(dt, left_eye->transformation());

	right_eye->update(dt);
	right_eye->Audio::Listener::update(dt, right_eye->transformation());
}

void
RendererVR::draw(ObjSet& objs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// texture for left eye
	glBindTexture(GL_TEXTURE_2D, left_eye_texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	left_eye->enable();

 	// RenderScene( vr::Eye_Left );
 	glBindFramebuffer( GL_FRAMEBUFFER, left_eye_framebuffer );

	// draw stuff for left eye
	auto drawn_objs = draw_objs_in_world(objs);

	glBindTexture(GL_TEXTURE_2D, right_eye_texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	right_eye->enable();

 	// RenderScene( vr::Eye_Right );
 	glBindFramebuffer( GL_FRAMEBUFFER, right_eye_framebuffer );

	// draw stuff for right eye
	drawn_objs = draw_objs_in_world(objs);

	if ( m_pHMD )
	{
		vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)left_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
		vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)right_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );
	}
}

vector<std::shared_ptr<Glome::Drawable>>
RendererVR::draw_objs_in_world(ObjSet& objs)
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

		Vector4 world_pos = ptr->get_world_t().position();
		if(frustum.isIn(*ptr, world_pos)) {
			if(ptr->is_transparent()) {
				float dist = std::acos(cam_pos.dot(world_pos))
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
RendererVR::fill_minimap(const vector<std::shared_ptr<Glome::Drawable>>& objs, Camera &cam)
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
RendererVR::Viewport::update(float dt)
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

const QRot RendererVR::Viewport::cam_offset(
	yz_qrot(0.2) *
	zw_qrot(-0.015) *
	yw_qrot(-0.01)
);
