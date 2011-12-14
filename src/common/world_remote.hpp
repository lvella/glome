#pragma once

#include <unordered_map>

#include "world_remote.hpp"

class world_remote : public world_local
{
protected:
	std::unordered_map<ship*> remote_players;
};

