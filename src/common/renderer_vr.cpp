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
#include "renderer.hpp"
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

RendererVR::RendererVR(const vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, std::shared_ptr<vr::IVRSystem> pHMD) :
	Renderer(pp, audio_world)
{

	m_pHMD = pHMD;

    vr::EVRInitError eError = vr::VRInitError_None;
    
    if ( !vr::VRCompositor() )
    {
        std::cout << "Failed to init VR Compositor: %s" << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        exit(-1);
    }

	//-------------------------------------------------------------------------------------------------------------------------------------//
	//                                 I dont know how this stuff works yet                                                                //
	//-------------------------------------------------------------------------------------------------------------------------------------//

	// // get data dir path
	// auto fname = get_data_path("actions.json");

    // vr::VRInput()->SetActionManifestPath( Path_MakeAbsolute( fname.c_str(), Path_StripFilename( Path_GetExecutablePath() ) ).c_str() );

	// vr::VRInput()->GetActionHandle( "/actions/demo/in/HideCubes", &m_actionHideCubes );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/HideThisController", &m_actionHideThisController);
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/TriggerHaptic", &m_actionTriggerHaptic );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/AnalogInput", &m_actionAnalongInput );

	// vr::VRInput()->GetActionSetHandle( "/actions/demo", &m_actionsetDemo );

	// vr::VRInput()->GetActionHandle( "/actions/demo/out/Haptic_Left", &m_rHand[Left].m_actionHaptic );
	// vr::VRInput()->GetInputSourceHandle( "/user/hand/left", &m_rHand[Left].m_source );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/Hand_Left", &m_rHand[Left].m_actionPose );

	// vr::VRInput()->GetActionHandle( "/actions/demo/out/Haptic_Right", &m_rHand[Right].m_actionHaptic );
	// vr::VRInput()->GetInputSourceHandle( "/user/hand/right", &m_rHand[Right].m_source );
	// vr::VRInput()->GetActionHandle( "/actions/demo/in/Hand_Right", &m_rHand[Right].m_actionPose );

	//-------------------------------------------------------------------------------------------------------------------------------------//
	//                                                                 I dont know how this stuff works yet                                //
	//-------------------------------------------------------------------------------------------------------------------------------------//

	assert(pp.size() <= 4 && "I don't know how to draw more than 4 players on the screen!");
	int h = height / (pp.size() > 2 ? 2 : 1);
	int w = width / (pp.size() > 1 ? 2 : 1);

	for(int i = 0; i < pp.size(); ++i) {
		players.emplace_back(pp[i], (i%2) * w, height - (i/2 + 1) * h, w, h, audio_world);
	}

	Fire::set_width(w);

	Frustum::initializeAtOrigin(frustum_at_origin);
}

void
RendererVR::update(float dt)
{
	for(Viewport& v: players) {
		v.update(dt);
		v.Audio::Listener::update(dt, v.transformation());
	}
}

void
RendererVR::draw(ObjSet& objs)
{
    
    if ( m_pHMD )
	{
		// RenderControllerAxes();
		RenderStereoTargets();
		RenderCompanionWindow();

		vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
		vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(active = begin(players); active != end(players); ++active) {
		active->enable();

		auto drawn_objs = draw_objs_in_world(objs);

		MiniMap::draw(active->_x, active->_y, this,
			active->transformation().inverse(), drawn_objs
		);
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





void RenderCompanionWindow()
{
	glDisable(GL_DEPTH_TEST);
	glViewport( 0, 0, m_nCompanionWindowWidth, m_nCompanionWindowHeight );

	glBindVertexArray( m_unCompanionWindowVAO );
	glUseProgram( m_unCompanionWindowProgramID );

	// render left eye (first half of index array )
	glBindTexture(GL_TEXTURE_2D, leftEyeDesc.m_nResolveTextureId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glDrawElements( GL_TRIANGLES, m_uiCompanionWindowIndexSize/2, GL_UNSIGNED_SHORT, 0 );

	// render right eye (second half of index array )
	glBindTexture(GL_TEXTURE_2D, rightEyeDesc.m_nResolveTextureId  );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glDrawElements( GL_TRIANGLES, m_uiCompanionWindowIndexSize/2, GL_UNSIGNED_SHORT, (const void *)(uintptr_t)(m_uiCompanionWindowIndexSize) );

	glBindVertexArray( 0 );
	glUseProgram( 0 );
}



void RenderStereoTargets()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_MULTISAMPLE );

	// Left Eye
	glBindFramebuffer( GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );
 	RenderScene( vr::Eye_Left );
 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
	glDisable( GL_MULTISAMPLE );
	 	
 	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId );

    glBlitFramebuffer( 0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, 
		GL_COLOR_BUFFER_BIT,
 		GL_LINEAR );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );	

	glEnable( GL_MULTISAMPLE );

	// Right Eye
	glBindFramebuffer( GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );
 	RenderScene( vr::Eye_Right );
 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
 	
	glDisable( GL_MULTISAMPLE );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId );
	
    glBlitFramebuffer( 0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, 
		GL_COLOR_BUFFER_BIT,
 		GL_LINEAR  );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );
}