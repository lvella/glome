#include "thread_pool.hpp"
#include <boost/context/fiber_fcontext.hpp>
#include <thread>
#include <variant>

namespace ctx = boost::context;

ThreadPool::ThreadPool(unsigned size)
{
	if(!size) {
		size = std::max(1u, std::thread::hardware_concurrency() - 1);
	}

	threads.reserve(size);
	for(unsigned i = 0; i < size; ++i) {
		threads.emplace_back([this] {
			try {
				do_work();
			} catch (ctx::detail::forced_unwind e) {
				std::cerr << "######" << std::endl;
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

void ThreadPool::do_work()
{
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
}

ThreadPool globalThreadPool;
