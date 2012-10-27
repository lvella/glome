#pragma once

#include <functional>
#include "controller.hpp"

class ControllerLocal : public Controller
{
public:
	ControllerLocal(std::vector<Ship*>* pp, std::vector<Ship*>* bp, std::vector<AiController*>* ai);
	virtual ~ControllerLocal();
	virtual void update();

protected:

public:
	/** Events callbacks */
	std::function<void(int, int)> collision_cb;
	/** */
};

