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
		~async_caller()
		{
			// stop exec thread if running
			_stop_exec = true;

			// wait until function has finished running by waiting for a lock
			std::unique_lock<std::mutex> mutex(_queue_mutex);
			mutex.unlock();
		}

		//! Add a new \a callback to the \c _callbacks list. Will be invoked later.
		void add_function(std::function<void()>&& callback)
		{
			std::unique_lock<std::mutex> mutex(_queue_mutex);

			_callbacks.push_back(std::move(callback));

			mutex.unlock();

			_callbacks_exist.notify_one();
		}

		//! Process all current callbacks that have been queued up. Will block until all callbacks are processed.
		void exec_callbacks()
		{
			std::unique_lock<std::mutex> mutex(_queue_mutex);

			while (!_callbacks.empty())
			{
				const auto callback = std::move(_callbacks.at(0));
				_callbacks.pop_front();

				// unlock when running callback because callback might take some time?
				mutex.unlock();

				callback();

				mutex.lock();
			}

			mutex.unlock();
		}

		//! Non returning function, will wait for callbacks to be placed onto queue before running.
		void exec()
		{
			while (!_stop_exec)
			{
				std::unique_lock<std::mutex> mutex(_queue_mutex);
				_callbacks_exist.wait(mutex);

				// duplicate code, bad I know :( sorry don't kill me
				while (!_callbacks.empty())
				{
					const auto callback = std::move(_callbacks.at(0));
					_callbacks.pop_front();

					// unlock when running callback because callback might take some time?
					mutex.unlock();

					callback();

					mutex.lock();
				}

				mutex.unlock();
			}
		}

	private:
		std::mutex _queue_mutex = {};
		std::atomic_bool _stop_exec = false;
		std::condition_variable _callbacks_exist = {};
		std::deque<std::function<void()>> _callbacks = {};
	};
}