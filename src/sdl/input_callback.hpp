#pragma once

#include <functional>

namespace Input
{

typedef std::function<void (int, float)> pfunction;

struct input_callback
{
	input_callback();
	input_callback(int nc, pfunction nf, int ne) : cid(nc), f(nf), e(ne) {};

	int cid;
	pfunction f;
	int e;
};

}

