#include <common/timer.hpp>
#include <thread>

using namespace lib::common;

timer::timer(async_caller& async_caller, std::function<void()>&& callback, timer_mode_e timer_mode, std::chrono::milliseconds timeout) :
	_async_caller(async_caller), _timer_mode(timer_mode), _is_running(false), _callback(std::move(callback)), _timeout(timeout)
{
}

timer::~timer()
{
	reset_timer();
	_timeout_thread.join();
}

void timer::start_timer()
{
	_is_running = true;

	_timeout_thread = std::thread([this]()
	{
		while (_is_running)
		{
			// timer timeout ;)
			std::unique_lock<std::mutex> mutex(_exit_mutex);
			_exit_timer_thread.wait_for(mutex, _timeout);

			if (!_is_running)
			{
				break;
			}

			// place the function onto the async caller, async caller will invoke the callback
			_async_caller.add_function([this](){ _callback(); });

			if (_timer_mode == timer_mode_e::ONE_SHOT)
			{
				break;
			}
		} });
}

void timer::reset_timer()
{
	_is_running = false;
	_exit_timer_thread.notify_all();
}