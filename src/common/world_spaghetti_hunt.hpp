#pragma once

#include "world.hpp"

#include <memory>

#include "meridian.hpp"
#include "score_renderer.hpp"
#include "renderer_vr.hpp"
#include "openvr.h"

class WorldSpaghettiHunt final:
	public World,
	public NonCopyable
{
public:
	WorldSpaghettiHunt(vr::IVRSystem* hmd);

private:
	bool is_alive() override;
	bool was_alive = true;

	std::shared_ptr<ShipController> controller;
	std::weak_ptr<Ship> player;
	std::shared_ptr<Meridians> meridians;
	vr::IVRSystem* hmd;

	FullViewRenderer<ScoreGameOverRenderer> *game_over_render = nullptr;
};
