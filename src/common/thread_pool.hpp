#pragma once

#include <atomic>
#include <limits>
#include <thread>
#include <future>
#include <functional>
#include <variant>
#include <iostream>

#include <blockingconcurrentqueue.h>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	ThreadPool(unsigned size = 0);
	~ThreadPool();

	void try_execute_one_task();
private:
	struct ThreadExit {};
	using QueueElement = std::variant<ThreadExit, Task>;

	friend class TaskAdder;

	void add_task(Task&& t);

	moodycamel::BlockingConcurrentQueue<QueueElement> queue;
	std::vector<std::thread> threads;
};

class TaskSet
{
private:
	TaskSet(ThreadPool &tp):
		tp(tp)
	{}

	struct SubtractOnExit {
		SubtractOnExit(TaskSet& ts):
			ts(ts)
		{}

		~SubtractOnExit()
		{
			uint32_t val = --ts.counter;
			assert(val != std::numeric_limits<uint32_t>::max());
		}

		TaskSet &ts;
	};

	ThreadPool &tp;
	std::atomic_uint32_t counter{0};

	friend class TaskAdder;

	template<class Func>
	friend void parallel_run_and_wait(ThreadPool &tp, Func&& start_function);
};

class TaskAdder {
public:
	template<class Func>
	void operator()(Func&& f)
	{
		ts.counter++;

		ts.tp.add_task([f=std::move(f), &ts=ts](){
			TaskSet::SubtractOnExit guard(ts);
			f();
		});
	}
private:
	TaskAdder(TaskSet &ts):
		ts(ts)
	{}

	TaskSet &ts;

	template<class Func>
	friend void parallel_run_and_wait(ThreadPool &tp, Func&& start_function);
};

template<class Func>
void parallel_run_and_wait(ThreadPool &tp, Func&& start_function)
{
	TaskSet ts(tp);
	{
		ts.counter++;
		TaskSet::SubtractOnExit guard(ts);
		start_function(TaskAdder(ts));
	}

	// Run tasks from thread pool or busy waits, if there are none.
	// TODO: devise some way of sleep waiting on both the task set
	// and on thread pool queue.
	while(ts.counter.load()) {
		tp.try_execute_one_task();
	}
}

extern ThreadPool globalThreadPool;
