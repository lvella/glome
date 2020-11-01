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
	bool is_alive() override;

	std::vector<std::weak_ptr<Ship>> players;
	std::shared_ptr<Meridians> meridians;
	vr::IVRSystem* m_pHMD = NULL;
};
