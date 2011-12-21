#pragma once

#include <functional>

#include "controller.hpp"
#include "ship_ai.hpp"

class controller_local : public controller
{
public:
	virtual void update();
	controller_local(std::vector<ship*>* pp, std::vector<ship_ai*>* pvb);

protected:
	std::vector<ship_ai*>* bots;

public:
	/** Events callbacks */
	std::function<void(int, int)> collision_cb;
	/** */
};

