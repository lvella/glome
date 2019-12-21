#pragma once

class RunContext
{
public:
	virtual ~RunContext() {};

	virtual void setup_display() = 0;

	virtual void draw() = 0;
	virtual void update() = 0;
};
