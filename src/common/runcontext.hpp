#pragma once

class RunContext
{
public:
	virtual ~RunContext() {};

	virtual void setup_display() = 0;

	virtual void draw() = 0;

	/** Advances context by one frame.
	 *
	 * @returns true if context is still active
	 */
	virtual bool update(float dt) = 0;
};
