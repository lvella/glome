#include "thread_pool.hpp"

#include <thread>
#include <variant>

ThreadPool::ThreadPool(unsigned size)
{
	if(!size) {
		size = std::max(1u, std::thread::hardware_concurrency() - 1);
	}

	threads.reserve(size);
	for(unsigned i = 0; i < size; ++i) {
		threads.emplace_back([this](){
			moodycamel::ConsumerToken tok(queue);

			for(;;) {
				QueueElement e;
				queue.wait_dequeue(tok, e);

				try {
					std::get<Task>(e)();
				} catch (const std::bad_variant_access&) {
					// ThreadExit received, bail.
					break;
				}
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	for(unsigned i = 0; i < threads.size(); ++i) {
		queue.enqueue(ThreadExit{});
	}

	for(auto& t: threads) {
		t.join();
	}
}

void ThreadPool::add_task(Task&& t)
{
	queue.enqueue(t);
}

void ThreadPool::try_execute_one_task()
{
	QueueElement e;
	if(!queue.try_dequeue(e)) {
		return;
	}

	try {
		std::get<Task>(e)();
	} catch (const std::bad_variant_access&) {
		// WTF??? This is only possible if destructor has been called!
		assert(false);
	}
}

ThreadPool globalThreadPool;
