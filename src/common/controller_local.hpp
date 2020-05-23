#pragma once

#include <functional>
#include "controller.hpp"

class ControllerLocal : public Controller
{
public:
	ControllerLocal(std::vector<Ship*>&& pp,
			std::vector<Ship*>&& bp,
		 std::vector<AiController*>&& ai):
		 Controller(std::move(pp), std::move(bp), std::move(ai))
		 {}
	virtual ~ControllerLocal() {};
	void update() override;

protected:

public:
	/** Events callbacks */
	std::function<void(int, int)> collision_cb;
	/** */
};

