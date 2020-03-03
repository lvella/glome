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

private:
	struct ThreadExit {};
	using QueueElement = std::variant<ThreadExit, Task>;

	friend class TaskSet;

	void add_task(Task&& t);

	moodycamel::BlockingConcurrentQueue<QueueElement> queue;
	std::vector<std::thread> threads;
};

class TaskSet
{
public:
	template<class Func>
	void add_task(ThreadPool &tp, Func&& f)
	{
		++counter;

		tp.add_task([f=std::move(f), this](){
			struct OnExit {
				OnExit(TaskSet& ts):
					ts(ts)
				{}

				~OnExit()
				{
					ts.notify_task_ended();
				}

				TaskSet &ts;
			} on_exit(*this);

			f();
		});
	}

	std::future<void> get_future()
	{
		return promise.get_future();
	}

private:
	void notify_task_ended()
	{
		uint32_t val = --counter;
		assert(val != std::numeric_limits<uint32_t>::max());

		if(val == 0) {
			promise.set_value();
		}
	}

	std::promise<void> promise;
	std::atomic_uint32_t counter{0};
};

extern ThreadPool globalThreadPool;
