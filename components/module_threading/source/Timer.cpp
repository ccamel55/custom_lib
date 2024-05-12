#include <module_threading/Timer.hpp>

using namespace lib::threading;

Timer::Timer(
    timer_mode mode,
    std::chrono::milliseconds timeout
)   : _mode(mode)
    , _timeout(timeout)
    , _running(false) {

}

Timer::~Timer() {
    reset();
}

void Timer::start() {
    if (_running) {
        return;
    }

    _running = true;
    _exec_start_time = std::chrono::system_clock::now();

    _thread = std::thread([&]{
        while(true) {
            // Wait until we receive something from the cv or until we reach our timeout.
            std::unique_lock<std::mutex> lock(_callback_mutex);
            _thread_cv.wait_until(lock, _exec_start_time + _timeout);

            if (!_running) {
                break;
            }

            // We keep track of exec start time and use it to calculate run interval. This allows us to have consistent
            // intervals regardless of function runtime.
            _exec_start_time = std::chrono::system_clock::now();

            for (const auto& fn: _callbacks) {
                lock.unlock();

                fn();

                lock.lock();
            }

            // Leave now if we are not reloading.
            if (_mode != timer_mode::RELOADING) {
                _running = false;
                break;
            }
        }
    });
}

void Timer::reset() {
    // Tell thread to piss off
    if (_running) {
        _running = false;
        _thread_cv.notify_all();
    }

    // Wait for it to stop
    if (_thread.joinable()) {
        _thread.join();
    }
}

void Timer::emplace(std::function<void()>&& callback) {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _callbacks.emplace_back(std::move(callback));
}

void Timer::clear() {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _callbacks.clear();
}
