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

// void
// RendererVR::setup_display()
// {
// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glViewport(0, 0, width, height);
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// }

RendererVR::RendererVR(const vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, /*std::shared_ptr<vr::IVRSystem>*/ vr::IVRSystem* const pHMD) :
	Renderer(pp, audio_world)
{	
	assert(pp.size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (pp.size() > 2 ? 2 : 1);
	int w = width / (pp.size() > 1 ? 2 : 1);

	for(int i = 0; i < pp.size(); ++i) {
		players.emplace_back(pp[i], (i%2) * w, height - (i/2 + 1) * h, w, h, audio_world);
	}

	Fire::set_width(w);

	Frustum::initializeAtOrigin(frustum_at_origin);

	m_pHMD = pHMD;

    vr::EVRInitError eError = vr::VRInitError_None;
    
    if ( !vr::VRCompositor() )
    {
        std::cout << "Failed to init VR Compositor: %s" << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        exit(-1);
    }

	// framebuffer for offscreen rendering
	glGenFramebuffers(1, &temp_framebuffer);
	// textures to store rendering for both eyes
	glGenTextures(1, &left_eye_texture);
	glGenTextures(1, &right_eye_texture);
}

void
RendererVR::update(float dt)
{
	vr::VRActiveActionSet_t actionSet = { 0 };
	// actionSet.ulActionSet = m_actionsetDemo;
	vr::VRInput()->UpdateActionState( &actionSet, sizeof(actionSet), 1 );

	players[0].update(dt);
	players[0].Audio::Listener::update(dt, players[0].transformation());
}

void
RendererVR::draw(ObjSet& objs)
{
	// assert(players.size() == 1);
	// for(active = begin(players); active != end(players); ++active)
	// 	active->enable();
	active = begin(players);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/***********************************************************/
	/*               LEFT EYE                                  */
	/***********************************************************/

	glBindFramebuffer(GL_FRAMEBUFFER, temp_framebuffer);
	// "Bind" this texture, so future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, left_eye_texture);

	// give an empty image to the texture (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, active->_w, active->_h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	// apparently this is also needed
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// set the texture as out colour attachment #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, left_eye_texture, 0);

	// TODO: move camera slightly to the left

	// render scene for left eye
	active->enable();

	// draw stuff for left eye
	auto drawn_objs = draw_objs_in_world(objs);

	//debug code for dumping texture to disk
	// std::cout << "Frame dimensions: " << active->_w << "x" << active->_h << std::endl;
	// std::cout << active->_w << " * " << active->_h << " * " << sizeof(GL_UNSIGNED_BYTE) << " * 3  = "
	// 	<< active->_w * active->_h * sizeof(GL_UNSIGNED_BYTE) * 3
	// 	<< std::endl;
	// size_t im_size = active->_w * active->_h;
	// char* pixels = (char*) malloc(sizeof(GL_UNSIGNED_BYTE) * 3 * im_size);
	// glReadPixels(0,0, active->_w, active->_h, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixels);
	// FILE* f_image = fopen("texture_left.rgb", "wb");
	// fwrite(pixels, 3 * im_size, sizeof(GL_UNSIGNED_BYTE), f_image);
	// fclose(f_image);
	// free(pixels);
	// // exit(0);

	/***********************************************************/
	/*               RIGHT EYE                                 */
	/***********************************************************/

	glBindFramebuffer(GL_FRAMEBUFFER, temp_framebuffer);
	// "Bind" this texture, so future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, right_eye_texture);

	// give an empty image to the texture (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, active->_w, active->_h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	// apparently this is also needed
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// set the texture as out colour attachment #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, right_eye_texture, 0);
	
	// TODO: move camera slightly to the left

	// render scene for left eye
	active->enable();

	// draw stuff for right eye
	drawn_objs = draw_objs_in_world(objs);

	//debug code for dumping texture to disk
	// std::cout << "Frame dimensions: " << active->_w << "x" << active->_h << std::endl;
	// std::cout << active->_w << " * " << active->_h << " * " << sizeof(GL_UNSIGNED_BYTE) << " * 3  = "
	// 	<< active->_w * active->_h * sizeof(GL_UNSIGNED_BYTE) * 3
	// 	<< std::endl;
	// im_size = active->_w * active->_h;
	// pixels = (char*) malloc(sizeof(GL_UNSIGNED_BYTE) * 3 * im_size);
	// glReadPixels(0,0, active->_w, active->_h, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixels);
	// f_image = fopen("texture_right.rgb", "wb");
	// fwrite(pixels, 3 * im_size, sizeof(GL_UNSIGNED_BYTE), f_image);
	// fclose(f_image);
	// free(pixels);
	// exit(0);

	/***********************************************************/
	/*               VR stuff                                  */
	/***********************************************************/

	if ( m_pHMD )
	{
		vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
		
		std::cout << "Submit frame to VRCompositor..." << '\n';
		vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)left_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)right_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );

		vr::VRCompositor()->PostPresentHandoff();
	}
}

// vector<std::shared_ptr<Glome::Drawable>>
// RendererVR::draw_objs_in_world(ObjSet& objs)
// {
// 	Camera camera(active->transformation());
// 	SpecsTracker specs(camera);

// 	const Vector4 cam_pos = active->transformation().inverse().position();

// 	vector<std::shared_ptr<Glome::Drawable>> remaining_objs;
// 	vector<std::pair<float, Glome::Drawable*>> transparent_objs;

// 	Frustum frustum = active->transformation().inverse() * frustum_at_origin;

// 	for(auto iter = objs.begin(); iter != objs.end();) {
// 		auto ptr = iter->second.lock();
// 		if(!ptr) {
// 			iter = objs.erase(iter);
// 			continue;
// 		}

// 		Vector4 world_pos = ptr->get_world_t().position();
// 		if(frustum.isIn(*ptr, world_pos)) {
// 			if(ptr->is_transparent()) {
// 				float dist = std::acos(cam_pos.dot(world_pos))
// 					- ptr->get_radius();
// 				assert(!std::isnan(dist));
// 				transparent_objs.push_back({dist, ptr.get()});
// 			} else {
// 				specs.maybe_set(iter->first);
// 				ptr->draw(camera);
// 			}
// 		}

// 		remaining_objs.emplace_back(std::move(ptr));
// 		++iter;
// 	}

// 	std::sort(transparent_objs.begin(), transparent_objs.end(),
// 		[] (auto& a, auto& b) {
// 			return a.first > b.first;
// 		}
// 	);

// 	auto sorted_projs = Projectile::cull_sort_from_camera(camera);
// 	Projectile::draw_many(sorted_projs, camera);

// 	for(auto &pair: transparent_objs) {
// 		auto& obj = *pair.second;
// 		specs.maybe_set(&obj.get_draw_specs());
// 		obj.draw(camera);
// 	}

// 	DustField::draw(camera);

// 	return remaining_objs;
// }

// void
// RendererVR::fill_minimap(const vector<std::shared_ptr<Glome::Drawable>>& objs, Camera &cam)
// {
// 	std::shared_ptr<Glome::Drawable> curr = active->t.lock();

// 	// TODO: This rendering is slow. Using GL_POINTS may be much faster.
// 	// Probably so insignificant it is not worth the effort.
// 	for(auto &obj: objs) {
// 		if(obj != curr)
// 			obj->minimap_draw(cam);
// 	}
// }

// void
// RendererVR::Viewport::update(float dt)
// {
// 	QRot new_trans;
// 	if(auto ptr = t.lock()) {
// 		new_trans = cam_offset * ptr->get_t().inverse();
// 	} else {
// 		new_trans = cam_hist.front().t;
// 	}

// 	cam_hist.push_back({dt, new_trans});

// 	while(cam_hist.front().dt <= dt) {
// 		dt -= cam_hist.front().dt;
// 		curr_qrot = cam_hist.front().t;

// 		cam_hist.pop_front();
// 		assert(!cam_hist.empty());
// 	}

// 	PathPoint& next = cam_hist.front();
// 	const float slerp_factor = dt / next.dt; // range [0, 1]

// 	curr_qrot = nlerp(curr_qrot, next.t, slerp_factor);
// 	next.dt -= dt;

// }

// const QRot RendererVR::Viewport::cam_offset(
// 	yz_qrot(0.2) *
// 	zw_qrot(-0.015) *
// 	yw_qrot(-0.01)
// );

// void read_framebuffer_onto_texture(int fboIn, int textureOut)
// {
//     // Bind input FBO + texture to a color attachment
//     glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIn);
//     glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIn, 0);
//     glReadBuffer(GL_COLOR_ATTACHMENT0);

//     // Bind destination FBO + texture to another color attachment
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboOut);
//     glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureOut, 0);
//     glDrawBuffer(GL_COLOR_ATTACHMENT1);

//     // specify source, destination drawing (sub)rectangles.
//     glBlitFramebuffer(0, 0, width, height,
//                         0, 0, width, height,
//                         GL_COLOR_BUFFER_BIT, GL_NEAREST);

//     // unbind the color attachments
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
// }
