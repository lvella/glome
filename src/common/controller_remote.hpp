#pragma once

#include <unordered_map>

#include "controller_local.hpp"

class ControllerRemote : public ControllerLocal
{
public:
	ControllerRemote() {};
	virtual ~ControllerRemote() {};

	void update();
	ControllerRemote(std::vector<Ship*>* pp, std::unordered_map<int, Ship*>* prp);

protected:
	std::unordered_map<int, Ship*>* remote_players;
};

