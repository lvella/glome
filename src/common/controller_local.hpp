#pragma once

#include <functional>

#include "controller.hpp"
#include "ship_ai.hpp"

class ControllerLocal : public Controller
{
public:
	virtual void update();
	ControllerLocal(std::vector<Ship*>* pp, std::vector<ShipAI*>* pvb);

protected:
	std::vector<ShipAI*>* bots;

public:
	/** Events callbacks */
	std::function<void(int, int)> collision_cb;
	/** */
};

