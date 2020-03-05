#pragma once

#include <atomic>
#include <boost/context/fiber_fcontext.hpp>
#include <exception>
#include <limits>
#include <thread>
#include <future>
#include <functional>
#include <variant>
#include <iostream>

#include <boost/context/fiber.hpp>
#include <blockingconcurrentqueue.h>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	ThreadPool(unsigned size = 0);
	~ThreadPool();

private:
	struct ResumeFiber
	{
		boost::context::fiber fiber;
	};

	void do_work();

	struct ThreadExit {};
	using QueueElement = std::variant<ThreadExit, Task>;

	friend class TaskAdder;
	friend class TaskSet;

	template<class Func>
	friend void parallel_run_and_wait(ThreadPool &tp, Func&& start_function);

	void add_task(Task&& t);

	moodycamel::BlockingConcurrentQueue<QueueElement> queue;
	std::vector<std::thread> threads;
	std::vector<boost::context::fiber> original_fibers;
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

		~SubtractOnExit() noexcept(false)
		{
			uint32_t val = --ts.counter;
			assert(val != std::numeric_limits<uint32_t>::max());

			if(val == 0) {
				throw ThreadPool::ResumeFiber{std::move(ts.orignal_fiber)};
			}
		}

		TaskSet &ts;
	};

	ThreadPool &tp;
	std::atomic_uint32_t counter{0};
	boost::context::fiber orignal_fiber;

	friend class TaskAdder;

	template<class Func>
	friend void parallel_run_and_wait(ThreadPool &tp, Func&& start_function);
};

class TaskAdder {
public:
	template<class Func>
	void operator()(Func&& f)
	{
		uint32_t val = ++ts.counter;

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

	ts.counter++;
	start_function(TaskAdder(ts));

	namespace ctx = boost::context;
	ctx::fiber thread_worker([&ts] (ctx::fiber&& original) {
		ts.orignal_fiber = std::move(original);

		try {
			TaskSet::SubtractOnExit{ts};
			ts.tp.do_work();
		} catch (ThreadPool::ResumeFiber &rf) {
			return std::move(rf.fiber);
		}
		assert(0);
	});
	std::move(thread_worker).resume();
}

extern ThreadPool globalThreadPool;
