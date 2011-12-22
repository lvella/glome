#pragma once

#include <unordered_map>

#include "world_remote.hpp"

class WorldRemote : public WorldLocal
{
public
	WorldRemote() {};
	virtual ~WorldRemote() {};

protected:
	std::unordered_map<int, Ship*> remote_players;
};

