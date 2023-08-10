#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>
#include <functional>

namespace lib::common
{
	//! Async function caller.
	class async_caller
	{
	public:
		~async_caller();

		//! Add a new \a callback to the \c _callbacks list. Will be invoked later.
		void add_function(std::function<void()>&& callback);

		//! Process all current callbacks that have been queued up. Will block until all callbacks are processed.
		void exec_callbacks();

		//! Non returning function, will wait for callbacks to be placed onto queue before running.
		void spawn_exec_thread();

	private:
		void exec_callbacks_internal(std::unique_lock<std::mutex>& mutex);

	private:
		std::atomic_bool _caller_thread_running = false;

		std::mutex _callbacks_mutex = {};
		std::condition_variable _received_callback = {};

		std::thread _async_caller_thread = {};
		std::deque<std::function<void()>> _callbacks = {};
	};
}