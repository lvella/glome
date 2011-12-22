#pragma once

#include <unordered_map>

#include "world_remote.hpp"

class WorldRemote : public WorldLocal
{
protected:
	std::unordered_map<Ship*> remote_players;
};

