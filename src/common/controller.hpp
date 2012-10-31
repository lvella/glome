#pragma once

#include <vector>

#include "ship.hpp"

class Controller : public Updatable
{
public:
	Controller(std::vector< Ship* >&& pp,
		   std::vector< Ship* >&& bp,
	    std::vector< AiController* >&& ai):
	    players(std::move(pp)),
	    bots(std::move(bp)),
	    ai_controllers(std::move(ai))
	    {}
	virtual ~Controller() {};

	virtual void update() = 0;

protected:
	std::vector<Ship*> players;
	std::vector<Ship*> bots;
	std::vector<AiController*> ai_controllers;
};

