#pragma once

#include <functional>

namespace Input
{

using pfunction = std::function<void (int, float)>;

struct input_callback
{
	input_callback();
	input_callback(int nc, pfunction nf, int ne) : cid(nc), f(nf), e(ne) {};

	int cid;
	pfunction f;
	int e;
};

}

