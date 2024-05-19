#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

namespace lib::threading
{
constexpr uint8_t DEFAULT_PRIORITY = 50;

class ThreadPool {
    struct queue_object_t {
        queue_object_t() = default;
        queue_object_t(uint8_t priority, std::function<void()>&& function)
            : priority(priority)
            , function(std::move(function)) {
        }

        uint8_t priority = DEFAULT_PRIORITY;
        mutable std::function<void()> function = nullptr;

        // Min heap, aka lower the number, lower the priority.
        bool operator()(const queue_object_t& l, const queue_object_t& r) const {
            return l.priority > r.priority;
        }
    };

public:
    //! Create a new thread pool object.
    //! \param max_threads The maximum number of threads to use in the thread pool. Defaults to 0.
    explicit ThreadPool(size_t max_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    //! Add a new job to the thread pool
    //! \param priority Function priority, the lower the number, the higher the priority.
    //! \param function Function to call in the thread pool.
    template <typename T> requires (std::is_null_pointer_v<T> == false)
    void emplace(uint8_t priority, T&& function) {
        std::unique_lock<std::mutex> lock(_job_queue_mutex);
        _job_queue.emplace(priority, std::forward<T>(function));
    }

    //! Add a new job to the thread pool
    //! \param function Function to call in the thread pool.
    template <typename T> requires (std::is_null_pointer_v<T> == false)
    void emplace(T&& function) {
        emplace(DEFAULT_PRIORITY, std::forward<T>(function));
    }

    //! Add a new job to the thread pool and return a future for that job.
    void emplace_future();

    //! Remove all queued jobs from the thread pool.
    void clear();

    //! Returns the number of jobs queued in the thread pool.
    [[nodiscard]] size_t size() const;

    //! Whether the thread pool is empty or not.
    [[nodiscard]] bool empty() const;

public:
    size_t _num_threads = 0;
    std::atomic<bool> _running = false;

    std::condition_variable _threads_update = {};
    std::vector<std::thread> _threads = {};

    mutable std::mutex _job_queue_mutex = {};
    std::priority_queue<queue_object_t, std::deque<queue_object_t>, queue_object_t> _job_queue = {};

};
}