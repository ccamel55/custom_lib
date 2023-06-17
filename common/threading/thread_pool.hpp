#pragma once

#include <singleton.hpp>

#include <mutex>
#include <thread>
#include <condition_variable>

#include <queue>
#include <vector>
#include <functional>

namespace lib::common {

    // modified version of thread_pool_light
    // https://github.com/bshoshany/thread-pool/blob/master/BS_thread_pool_light.hpp
    using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;

    class thread_pool : public singleton<thread_pool> {
    public:
        void spawn_threads(std::function<void()> worker_constructor = nullptr, std::function<void()> worker_destructor = nullptr);
        void kill_threads();
        void wait_for_task();

        template <typename F, typename... A>
        void add_task(F&& task, A&&... args) {

            std::function<void()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...); {

                const std::scoped_lock task_lock(_tasks_mutex);
                _tasks.push(task_function);
            }

            _task_count++;
            _cv_task_available.notify_one();
        }
    private:
        static void worker_thread(void* param);

        std::function<void()> _worker_constructor = nullptr;
        std::function<void()> _worker_destructor = nullptr;

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