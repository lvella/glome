#pragma once

#include "controller.hpp"
#include "ship_ai.hpp"

class controller_local : public controller
{
public:
	controller_local(std::vector<ship*> pp, std::vector<ship_ai*>* pvb);

protected:
	std::vector<ship_ai*>* bots;
};

