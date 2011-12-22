#pragma once

#include "controller_local.hpp"

class ControllerRemote : public ControllerLocal
{
public:
	void update();
	ControllerRemote(std::vector<Ship*> pp, std::unordered_map<Ship*>* prp);

protected:
	std::unordered_map<Ship*>* remote_players;
};

