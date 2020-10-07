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

RendererVR::RendererVR(const vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, vr::IVRSystem* const pHMD) :
	Renderer(pp, audio_world)
{
	players.emplace_back(pp[0], 0, 0, width, height, audio_world);
	active = begin(players);

	Fire::set_width(width);

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
	auto original_transform = active->transformation();

	// left eye
	draw_eye(left_eye_texture, temp_framebuffer, Eye::left, original_transform, objs);

	// right eye
	draw_eye(right_eye_texture, temp_framebuffer, Eye::right, original_transform, objs);

	// vr stuff
	if ( m_pHMD )
	{
		vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

		vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)left_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)right_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );

		vr::VRCompositor()->PostPresentHandoff();
	}

	// restore original transform (before VR distortion)
	active->curr_qrot = original_transform;
}

std::vector<std::shared_ptr<Glome::Drawable>>
RendererVR::draw_eye(const GLuint texture, const GLuint framebuffer, const Eye eye, const QRot original_transform, ObjSet& objs)
{

	// bind the frambebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// give an empty image to the texture (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, active->_w, active->_h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// apparently this is also needed
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// set the texture as colour attachment #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	// move camera slightly
	//    if left eye, to the left
	//    if left right, to the right
	active->curr_qrot = xw_qrot((eye * -1) * 0.005) * original_transform;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render scene for left eye
	active->enable();

	// draw stuff for left eye
	auto drawn_objs = draw_objs_in_world(objs);

	return drawn_objs;

}
