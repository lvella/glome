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
#include "fatal_error.hpp"

#include <algorithm>
#include <memory>
#include <iomanip>
#include <stdlib.h>

using namespace std;
using namespace Options;

RendererVR::RendererVR(std::weak_ptr<Ship> player,
		Audio::World &audio_world, vr::IVRSystem* const pHMD) :
	PlayerScoreRenderer(player, audio_world),
	m_pHMD{pHMD}
{
	assert(m_pHMD);

	m_pHMD->GetRecommendedRenderTargetSize(&width, &height);

	Fire::set_width(width);

	if ( !vr::VRCompositor() )
	{
		fatal_user_error("Failed to init VR Compositor.");
	}

	// framebuffer for offscreen rendering
	glGenFramebuffers(1, &temp_framebuffer);

	// textures to store rendering for both eyes
	GLuint textures[2];
	glGenTextures(2, textures);

	left_eye_texture = textures[0];
	right_eye_texture = textures[1];
}

RendererVR::~RendererVR()
{
	GLuint textures[2] = {left_eye_texture, right_eye_texture};
	glDeleteTextures(2, textures);

	glDeleteFramebuffers(1, &temp_framebuffer);
}

void
RendererVR::update(float dt)
{
	vr::VRActiveActionSet_t actionSet = { 0 };
	// actionSet.ulActionSet = m_actionsetDemo;
	vr::VRInput()->UpdateActionState( &actionSet, sizeof(actionSet), 1 );

	PlayerScoreRenderer::update(dt);
}

void
RendererVR::draw(ObjSet& objs)
{
	auto original_transform = transformation();
	glViewport(0, 0, width, height);

	// left eye
	draw_eye(left_eye_texture, temp_framebuffer, Eye::left, original_transform, objs);

	// right eye
	draw_eye(right_eye_texture, temp_framebuffer, Eye::right, original_transform, objs);

	// restore original transform (before VR distortion)
	curr_qrot = original_transform;

	// vr stuff
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)left_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)right_eye_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );

	vr::VRCompositor()->PostPresentHandoff();
}

void
RendererVR::draw_eye(const GLuint texture, const GLuint framebuffer, const Eye eye, const QRot original_transform, ObjSet& objs)
{
	// bind the frambebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// give an empty image to the texture (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

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
	curr_qrot = xw_qrot((eye * -1) * 0.005) * original_transform;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render scene for left eye
	PlayerScoreRenderer::draw(objs);
}
