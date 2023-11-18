#pragma once
#include <functional>
#include <mutex>
#include <vector>

namespace lib::threading
{
//! Signal handler, like linux but doesnt interrupt :(
template <typename... a> class signal_handler
{
public:
	// we usually only ever register callbacks during initialisation which means the cost of copying when the vector
	// expands is not going to be important
	~signal_handler()
	{
		// wait until function has finished running by waiting for a lock
		std::unique_lock<std::mutex> mutex(_queue_mutex);
		mutex.unlock();
	}

	//! Add a new \a callback to the \c _callbacks list.
	void register_callback(std::function<void(a...)>&& callback)
	{
		std::unique_lock<std::mutex> mutex(_queue_mutex);

		_callbacks.push_back(std::move(callback));

		mutex.unlock();
	}

	//! Execute all callbacks with exec parameters.
	void exec(a... Args)
	{
		std::unique_lock<std::mutex> mutex(_queue_mutex);

		for (const auto& callback : _callbacks)
		{
			mutex.unlock();

			callback(Args...);

			mutex.lock();
		}

		mutex.unlock();
	}

private:
	std::mutex _queue_mutex = {};
	std::vector<std::function<void(a...)>> _callbacks = {};
};
}  // namespace lib::threading