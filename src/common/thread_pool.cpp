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
			ctx::fiber worker([this] (ctx::fiber&& orig) {
				original_fibers.push_back(std::move(orig));

				try {
					do_work();
				} catch (ResumeFiber &rf) {
					return std::move(rf.fiber);
				}
				assert(0);
			});
			worker = std::move(worker).resume();
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
