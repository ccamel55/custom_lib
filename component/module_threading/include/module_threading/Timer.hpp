#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

namespace lib::threading
{
    enum class timer_mode {
        //! One shot timers execute their functions once when started.
        ONE_SHOT,

        //! Reloading timers execute their functions continuously when started.
        RELOADING
    };

    class Timer {
    public:
        Timer(
            timer_mode mode,
            std::chrono::milliseconds timeout
        );

        ~Timer();

        //! Start the timer.
        void start();

        //! Stop the timer.
        void reset();

        //! Add a new callback to the timer.
        //! Callbacks are ran in the timer thread therefore all callbacks should not take longer to execute than the
        //! timeout duration.
        //! \param callback Callback that will be registered to the timer.
        void emplace(std::function<void()>&& callback);

        //! Remove all callbacks from the timer.
        void clear();

    private:
        timer_mode _mode = timer_mode::ONE_SHOT;

        std::chrono::milliseconds _timeout                     = { };
        std::chrono::system_clock::time_point _exec_start_time = { };

        std::thread _thread                = { };
        std::atomic<bool> _running         = false;
        std::condition_variable _thread_cv = { };

        std::mutex _callback_mutex                    = { };
        std::vector<std::function<void()>> _callbacks = { };

    };
}
