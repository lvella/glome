#pragma once

#include "ship_controller.hpp"

class AiController : public ShipController
{
public:
	AiController();
	~AiController();
	void act();

protected:

};

