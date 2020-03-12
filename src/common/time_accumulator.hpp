#pragma once

#include <chrono>
#include <string>
#include <iostream>

class TimeAccumulator
{
public:
	TimeAccumulator(std::string name):
		name(name)
	{}

private:
	std::string name;
	double sum_time = 0;
	size_t count = 0;

	friend class TimeGuard;
};

class TimeGuard {
public:
	TimeGuard(TimeAccumulator &ta):
		ta(ta),
		start(std::chrono::steady_clock::now())
	{}

	~TimeGuard()
	{
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;

		ta.sum_time += elapsed_seconds.count();
		std::cout << ta.name <<  " avg " << ta.sum_time/ ++ta.count << "s\n";
	}

private:
	TimeAccumulator &ta;
	std::chrono::steady_clock::time_point start;
};
