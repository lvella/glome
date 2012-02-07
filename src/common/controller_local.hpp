#pragma once

#include <functional>
#include "controller.hpp"

class ControllerLocal : public Controller
{
public:
	ControllerLocal(std::vector<Ship*>* pp);
	~ControllerLocal();
	virtual void update();

protected:

public:
	/** Events callbacks */
	std::function<void(int, int)> collision_cb;
	/** */
};

