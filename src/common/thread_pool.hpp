#pragma once

#include <atomic>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <variant>
#include <boost/thread/latch.hpp>
#include <iostream>

#include <blockingconcurrentqueue.h>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	ThreadPool(unsigned size = 0);
	~ThreadPool();

	template<class Func>
	void parallel_run_and_wait(Func&& start_function);

	template<class Func>
	void run_in_all_other_threads(Func&& function);

	unsigned get_num_threads() const
	{
		return threads.size();
	}

private:
	void try_execute_one_task();

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
	friend class ThreadPool;
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

	friend class ThreadPool;
};

template<class Func>
void ThreadPool::parallel_run_and_wait(Func&& start_function)
{
	TaskSet ts(*this);
	{
		ts.counter++;
		TaskSet::SubtractOnExit guard(ts);
		start_function(TaskAdder(ts));
	}

	// Run tasks from thread pool or busy waits, if there are none.
	// TODO: devise some way of sleep waiting on both the task set
	// and on thread pool queue.
	while(ts.counter.load()) {
		try_execute_one_task();
	}
}

// Executes function in all threads except the caller.
// Function receives an unique value among all threads, from 0 to get_num_threads() - 1.
// Blocks all threads until all have executed the function, then return.
// Acts as a barrier.
template<class Func>
void ThreadPool::run_in_all_other_threads(Func&& function)
{
	auto latch = std::make_shared<boost::latch>(threads.size() + 1);

	for(unsigned i = 0; i < threads.size(); ++i) {
		add_task([=, &function] {
			function(i);

			latch->count_down_and_wait();
		});
	}

	latch->count_down_and_wait();
}

extern ThreadPool globalThreadPool;
