#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

#include <lib_threading/async_caller/async_caller.hpp>

namespace lib::threading
{
enum class timer_mode_e
{
	one_shot,
	reloading
};

//! Function timer, will delay the call of a function until timer expires. Does create a thread.
class timer
{
public:
	timer(
		async_caller& async_caller,
		std::function<void()>&& callback,
		timer_mode_e timer_mode,
		std::chrono::milliseconds timeout);
	~timer();

	//! Start timeout for timer, this will create a new thread which will put the callback onto the async caller.
	void start_timer();

	//! Reset timout, stop from calling callback if timer has started.
	void reset_timer();

private:
	async_caller& _async_caller;
	timer_mode_e _timer_mode = timer_mode_e::one_shot;

	std::atomic_bool _is_running = false;
	std::thread _timeout_thread = {};

	std::mutex _exit_mutex = {};
	std::condition_variable _exit_timer_thread = {};

	std::function<void()> _callback = nullptr;
	std::chrono::milliseconds _timeout = std::chrono::milliseconds(1000);
};
}  // namespace lib::threading