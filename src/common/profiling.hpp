#pragma once

#include <chrono>
#include <string>
#include <list>
#include <iostream>

#include "utils.hpp"

class TimeAccumulator: public NonCopyable
{
public:
	TimeAccumulator(std::string name):
		name(name)
	{}

	void print() const
	{
		std::cout << name <<  " avg " << sum_time / count << "s\n";
	}

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
		++ta.count;
	}

private:
	TimeAccumulator &ta;
	std::chrono::steady_clock::time_point start;
};


class ProfillingAggregator
{
public:
	ProfillingAggregator(std::chrono::milliseconds print_period):
		print_period(print_period)
	{}

	TimeAccumulator& newTimer(std::string name)
	{
		return tas.emplace_back(name);
	}

	void maybe_print()
	{
		auto now = std::chrono::steady_clock::now();
		if(now - last_print >= print_period && !tas.empty()) {
			last_print = now;

			std::cout << "\nTimings:\n";
			for(const auto& ta: tas) {
				ta.print();
			}
			std::cout.flush();
		}
	}

private:
	const std::chrono::milliseconds print_period;
	std::chrono::steady_clock::time_point last_print;
	std::list<TimeAccumulator> tas;
};

extern ProfillingAggregator globalProfiler;
