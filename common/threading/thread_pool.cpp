#include <common/threading/thread_pool.hpp>

using namespace lib::common;

void thread_pool::worker_thread(void* param)
{
	const auto thread_pool_data = reinterpret_cast<thread_pool*>(param);

	while (thread_pool_data->_running)
	{
		std::function<void()> task;
		std::unique_lock<std::mutex> task_lock(thread_pool_data->_tasks_mutex);

		thread_pool_data->_cv_task_available.wait(task_lock, [thread_pool_data]
		{ return !thread_pool_data->_tasks.empty() || !thread_pool_data->_running; });

		if (thread_pool_data->_running)
		{
			task = std::move(thread_pool_data->_tasks.front());
			thread_pool_data->_tasks.pop();

			task_lock.unlock();

			task();

			task_lock.lock();
			--thread_pool_data->_task_count;

			if (thread_pool_data->_waiting)
			{
				thread_pool_data->_cv_task_done.notify_one();
			}
		}
	}
}

void thread_pool::spawn_threads()
{
	if (std::thread::hardware_concurrency() > 0)
	{
		_thread_count = std::thread::hardware_concurrency();
	}
	else
	{
		_thread_count = 1;
	}

	_threads = std::make_unique<std::thread[]>(_thread_count);
	_running = true;

	for (concurrency_t i = 0; i < _thread_count; i++)
	{
		_threads[i] = std::thread(worker_thread, this);
	}
}

void thread_pool::kill_threads()
{
	wait_for_task();

	_running = false;
	_cv_task_available.notify_all();

	for (concurrency_t i = 0; i < _thread_count; i++)
	{
		_threads[i].join();
	}
}

void thread_pool::wait_for_task()
{
	_waiting = true;

	std::unique_lock<std::mutex> task_lock(_tasks_mutex);
	_cv_task_done.wait(task_lock, [this]
	{ return (_task_count == 0); });

	_waiting = false;
}