#pragma once

#include <thread>
#include <vector>

#include <lib_threading/thread_pool/job_collection.hpp>

namespace lib::threading
{
//! General purpose thread pool. Executes a pool of jobs.
class thread_pool
{
public:
	explicit thread_pool(size_t max_threads);
	~thread_pool();

	//! Restart all worker threads
	void restart_worker_threads();

	//! Kill all worker threads.
	void kill_worker_threads();

	//! Queue a collection of jobs onto the thread pool.
	void queue_collection(std::weak_ptr<job_collection>&& collection);

private:
	void start_worker_threads();

private:
	size_t _num_threads = 0;
	std::atomic_bool _running_workers = false;

	std::mutex _collection_queue_mutex = {};
	std::deque<std::weak_ptr<job_collection>> _collection_queue = {};

	std::condition_variable _recieved_collection = {};
	std::vector<std::thread> _worker_threads = {};

};
}  // namespace lib::threading