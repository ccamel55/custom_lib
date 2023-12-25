#include <lib_threading/thread_pool/job_collection.hpp>

#include <core_sdk/logger.hpp>

using namespace lib::threading;

void job_collection::queue_job(std::function<void()>&& function)
{
	std::unique_lock<std::mutex> mutex(_job_queue_mutex);
	_job_queue.push_back(std::move(function));
}

void job_collection::wait_for_finish()
{
	// once we leave scope mutex is automatically released, including if we return early
	std::unique_lock<std::mutex> mutex(_job_queue_mutex);

	if (_job_queue.empty())
	{
		return;
	}

	// wait until receive a conditional variable if we have no callbacks
	_finished_tasks.wait(mutex);
}

std::deque<std::function<void()>>& job_collection::get_job_queue()
{
	return _job_queue;
}

std::condition_variable& job_collection::get_finished_tasks_cv()
{
	return _finished_tasks;
}