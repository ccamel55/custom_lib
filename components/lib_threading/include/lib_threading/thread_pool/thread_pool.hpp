#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <thread>
#include <vector>

namespace lib::threading
{
//! General purpose thread pool. Executes a pool of jobs.
class thread_pool
{
public:
	~thread_pool();

	//! Spawn worker threads. Will determine the number of threads automatically.
	void spawn_threads(size_t max_threads);

	//! Kill all worker threads.
	void kill_threads();

	//! Queue a task onto the thread pool.
	void queue_task(std::function<void()>&& function);

	//! Wait until all thread pool tasks have been completed.
	void wait_for_tasks();

private:
	std::atomic_bool _worker_threads_running = false;
	std::atomic_bool _waiting_for_finish = false;

	std::mutex _tasks_mutex = {};
	std::condition_variable _received_tasks = {};
	std::condition_variable _finished_tasks = {};

	std::vector<std::thread> _worker_threads = {};
	std::deque<std::function<void()>> _thread_pool_tasks = {};
};
}  // namespace lib::threading