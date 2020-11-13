#pragma once

#include "world.hpp"

#include <memory>
#include "meridian.hpp"

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

	std::weak_ptr<Ship> player;
	std::shared_ptr<Meridians> meridians;
	vr::IVRSystem* hmd;
};
