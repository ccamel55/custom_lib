#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>

namespace lib::common
{
	// todo: rewrite this shit
	// modified version of thread_pool_light
	// https://github.com/bshoshany/thread-pool/blob/master/BS_thread_pool_light.hpp
	using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;

	//! Thread pool which can batch process jobs.
	class thread_pool
	{
	public:
		~thread_pool();

		void spawn_threads();
		void kill_threads();
		void wait_for_task();
		void add_task(std::function<void()>&& task);

		//		template <typename f, typename... a>
		//		void add_task(f&& task, a&&... args)
		//		{
		//			std::function<void()> task_function = std::bind(std::forward<f>(task), std::forward<a>(args)...);
		//			{
		//				const std::scoped_lock task_lock(_tasks_mutex);
		//				_tasks.push(task_function);
		//			}
		//
		//			_task_count++;
		//			_cv_task_available.notify_one();
		//		}

	private:
		static void worker_thread(void* param);

		std::atomic<bool> _running = false;
		std::atomic<bool> _waiting = false;

		std::condition_variable _cv_task_available = {};
		std::condition_variable _cv_task_done = {};

		concurrency_t _thread_count = 0;
		std::atomic<size_t> _task_count = 0;

		mutable std::mutex _tasks_mutex = {};

		std::queue<std::function<void()>> _tasks = {};
		std::unique_ptr<std::thread[]> _threads = nullptr;
	};
}