#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <functional>
#include <condition_variable>

namespace lib::threading
{
class job_collection
{
public:
    //! Queue a task onto the thread pool.
	void queue_job(std::function<void()>&& function);

	//! Wait until all thread pool tasks have been completed.
	void wait_for_finish();

public:
	// used by thread pool, should be private but we cant friend static functions :(
	[[nodiscard]] std::deque<std::function<void()>>& get_job_queue();
	[[nodiscard]] std::condition_variable& get_finished_tasks_cv();

private:
	std::condition_variable _finished_tasks = {};

	std::mutex _job_queue_mutex = {};
    std::deque<std::function<void()>> _job_queue = {};
};
}