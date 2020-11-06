#pragma once

#include <memory>

#include "runcontext.hpp"
#include "world_dummy.hpp"

/** Controls high level game state. */
class GameStateMachine
{
public:
	virtual ~GameStateMachine() = default;

	virtual RunContext *get_next_context() = 0;
};

class DummyStateMachine final: public GameStateMachine
{
public:
	RunContext *get_next_context()
	{
		world = std::make_unique<WorldDummy>();
		return world.get();
	}

private:
	std::unique_ptr<WorldDummy> world;
};
