#pragma once

#include "controller_local.hpp"

class controller_remote : public controller_local
{
public:
	controller_remote(std::vector<ship*> pp, std::unordered_map<ship*>* prp);

protected:
	std::unordered_map<ship*>* remote_players;
};

