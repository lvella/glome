#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "frustum.hpp"

#include "renderer.hpp"
#include "openvr.h"

enum Eye {
	left=0,
	right
};

class RendererVR : public Renderer
{
public:
	using ObjSet = std::unordered_multimap<
		DrawSpecsBase*,
		std::weak_ptr<Glome::Drawable>
	>;

	RendererVR(const std::vector<std::weak_ptr<Ship>>& pp, Audio::World &audio_world, vr::IVRSystem* const pHMD);

	void update(float dt) override;
	void draw(ObjSet& objs) override;

	std::vector<std::shared_ptr<Glome::Drawable>>
	draw_eye(const GLuint texture, const GLuint framebuffer, const Eye eye, const QRot original_transform, ObjSet& objs);

protected:

	GLuint temp_framebuffer;

	GLuint left_eye_texture;
	GLuint right_eye_texture;

	vr::IVRSystem* m_pHMD;
};
