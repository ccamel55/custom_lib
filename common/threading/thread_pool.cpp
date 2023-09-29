#include <common/logger.hpp>
#include <common/threading/thread_pool.hpp>

using namespace lib::common;

thread_pool::~thread_pool()
{
	kill_threads();
}

void thread_pool::spawn_threads(size_t max_threads)
{
	if (_worker_threads_running)
	{
		lib_log_w("thread_pool: could not spawn worker threads, workers already exist");
		return;
	}

	// hardware_concurrency returns estimated number of threads a system has for concurrency
	size_t num_thread = std::thread::hardware_concurrency();

	if (num_thread == 0)
	{
		lib_log_w("thread_pool: current system does not support concurrency, spawning 1 worker thread");
		num_thread = 1;
	}

	// clamp number of max threads lol
	num_thread = std::min(num_thread, max_threads);

	// define worker thread task
	const auto worker_thread_task = [&]() -> void {
		while (_worker_threads_running)
		{
			std::unique_lock<std::mutex> mutex(_tasks_mutex);

			if (_thread_pool_tasks.empty())
			{
				// wait until receive a conditional variable if we have no callbacks
				_received_tasks.wait(mutex);
			}

			// if we have stopped in between loop check and receiving conditional variable exit
			if (!_worker_threads_running)
			{
				break;
			}

			while (!_thread_pool_tasks.empty())
			{
				const auto task = std::move(_thread_pool_tasks.at(0));
				_thread_pool_tasks.pop_front();

				mutex.unlock();

				task();

				mutex.lock();
			}

			if (_waiting_for_finish)
			{
				_finished_tasks.notify_all();
				_waiting_for_finish = false;
			}

			mutex.unlock();
		}
	};

	_worker_threads_running = true;

	for (size_t i = 0; i < num_thread; i++)
	{
		// create worker thread using thread_pool lambda
		_worker_threads.emplace_back(worker_thread_task);
	}

	lib_log_d("thread_pool: created worker threads");
}

void thread_pool::kill_threads()
{
	if (!_worker_threads_running)
	{
		return;
	}

	// stop processing data in thread
	_worker_threads_running = false;
	_received_tasks.notify_all();

	// wait until all threads terminates
	for (auto& thread : _worker_threads)
	{
		thread.join();
	}
}

void thread_pool::queue_task(std::function<void()>&& function)
{
	if (!_worker_threads_running)
	{
		lib_log_e("thread_pool: could not queue task, no worker threads exist");
		return;
	}

	std::unique_lock<std::mutex> mutex(_tasks_mutex);
	_thread_pool_tasks.push_back(std::move(function));

	// tell one of the worker threads to pick up job
	_received_tasks.notify_one();
}

void thread_pool::wait_for_tasks()
{
	if (!_worker_threads_running)
	{
		lib_log_w("thread_pool: did not wait for tasks, no worker threads exist");
		return;
	}

	std::unique_lock<std::mutex> mutex(_tasks_mutex);

	if (!_thread_pool_tasks.empty())
	{
		_waiting_for_finish = true;

		// wait until receive a conditional variable if we have no callbacks
		_finished_tasks.wait(mutex);
	}
}