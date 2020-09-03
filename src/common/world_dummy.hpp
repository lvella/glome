#pragma once

#include "world.hpp"

#include <memory>
#include "meridian.hpp"

#include "renderer_vr.hpp"

#include "openvr.h"

class WorldDummy final: public World
{
public:
	WorldDummy();
	~WorldDummy();

private:
	std::shared_ptr<Meridians> meridians;
	// std::shared_ptr<vr::IVRSystem> m_pHMD;
	vr::IVRSystem* m_pHMD = NULL;
};
