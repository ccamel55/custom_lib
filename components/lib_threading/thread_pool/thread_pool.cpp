#include <core_sdk/logger.hpp>
#include <lib_threading/thread_pool/thread_pool.hpp>

#include <cassert>

using namespace lib::threading;

thread_pool::thread_pool(size_t max_threads)
{
	// hardware_concurrency returns estimated number of threads a system has for concurrency
	_num_threads = std::thread::hardware_concurrency();

	if (_num_threads == 0)
	{
		lib_log_w("thread_pool: current system does not support concurrency, spawning 1 worker thread");
		_num_threads = 1;
	}

	_num_threads = std::min(_num_threads, max_threads);
	start_worker_threads();
}

thread_pool::~thread_pool()
{
	kill_worker_threads();
}

void thread_pool::restart_worker_threads()
{
	kill_worker_threads();
	start_worker_threads();
}

void thread_pool::kill_worker_threads()
{
	if (!_running_workers)
	{
		return;
	}

	// stop processing data in thread
	_running_workers = false;
	_recieved_collection.notify_all();

	// wait until all threads terminates
	for (auto& thread : _worker_threads)
	{
		thread.join();
	}

	_worker_threads.clear();
}

void thread_pool::queue_collection(std::weak_ptr<job_collection>&& collection)
{
	assert(_running_workers == true);

	std::unique_lock<std::mutex> mutex(_collection_queue_mutex);
	_collection_queue.push_back(std::move(collection));

	mutex.unlock();

	// tell all workers to check for collection items
	_recieved_collection.notify_all();
}

void thread_pool::start_worker_threads()
{
	const auto worker_thread_task = [&]() -> void
	{
		std::function<void()> task = nullptr;

		while (_running_workers)
		{
			std::unique_lock<std::mutex> mutex(_collection_queue_mutex);

			// pause the thread until we can aquire a collection
			if (_collection_queue.empty())
			{
				_recieved_collection.wait(mutex);
			}

			if (!_running_workers)
			{
				break;
			}

			// while there are collections left, grab an item and run it
			while (!_collection_queue.empty())
            {
				const auto& collection = _collection_queue.at(0).lock();

				if (!collection)
				{
					lib_log_e("thread_pool: tried to run a collection that has been deleted");
					assert(false);
				}

				auto& collection_job_queue = collection->get_job_queue();

				// if this collection is empty, then pop it off the queue
				if (collection_job_queue.empty())
				{
					collection->get_finished_tasks_cv().notify_all();
					_collection_queue.pop_front();

					continue;
				}

				task = std::move(collection_job_queue.at(0));

				// allow other workers to run while we process the task
				collection_job_queue.pop_front();
				mutex.unlock();

				task();

				mutex.lock();
            }

			mutex.unlock();
		}
	};

	assert(_running_workers == false);

	lib_log_d("thread_pool: starting {} worker threads", _num_threads);

	_running_workers = true;

	for (size_t i = 0; i < _num_threads; i++)
	{
		_worker_threads.emplace_back(worker_thread_task);
	}
}