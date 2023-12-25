#include <core_sdk/logger.hpp>
#include <lib_threading/async_caller/async_caller.hpp>

using namespace lib::threading;

async_caller::~async_caller()
{
	if (!_caller_thread_running)
	{
		return;
	}

	// stop processing data in thread
	_caller_thread_running = false;
	_received_callback.notify_all();

	// wait until thread terminates
	_async_caller_thread.join();
}

void async_caller::add_function(std::function<void()>&& callback)
{
	std::unique_lock<std::mutex> mutex(_callbacks_mutex);
	_callbacks.push_back(std::move(callback));

	// tell processing thread to do something if its waiting
	_received_callback.notify_one();
}

void async_caller::exec_callbacks()
{
	std::unique_lock<std::mutex> mutex(_callbacks_mutex);
	exec_callbacks_internal(mutex);
}

void async_caller::spawn_exec_thread()
{
	if (_caller_thread_running)
	{
		lib_log_w("async_caller: could not spawn exec thread, exec thread already exists");
		return;
	}

	const auto async_caller_thread = [&]() -> void {
		while (_caller_thread_running)
		{
			std::unique_lock<std::mutex> mutex(_callbacks_mutex);

			if (_callbacks.empty())
			{
				// wait until receive a conditional variable if we have no callbacks
				_received_callback.wait(mutex);
			}

			// if we have stopped in between loop check and receiving conditional variable exit
			if (!_caller_thread_running)
			{
				break;
			}

			exec_callbacks_internal(mutex);
		}
	};

	_caller_thread_running = true;
	_async_caller_thread = std::thread(async_caller_thread);

	lib_log_d("async_caller: created exec thread");
}

void async_caller::exec_callbacks_internal(std::unique_lock<std::mutex>& mutex)
{
	while (!_callbacks.empty())
	{
		const auto callback = std::move(_callbacks.at(0));
		_callbacks.pop_front();

		mutex.unlock();

		callback();

		mutex.lock();
	}

	mutex.unlock();
}