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
        queue_object_t(uint8_t priority, const std::shared_ptr<std::packaged_task<void()>>& function)
            : priority(priority)
            , function(function) {
        }

        uint8_t priority = DEFAULT_PRIORITY;
        mutable std::shared_ptr<std::packaged_task<void()>> function = nullptr;

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
    //! \param args Optional arguments to pass to the function.
    //! \return Future for function.
    template<typename Fn, typename... Args> requires (std::is_null_pointer_v<Fn> == false)
    std::future<void> emplace(uint8_t priority, Fn&& function, Args&&... args) {
        // We need to do this BS since std::packaged_task isn't copyable and the comparison operator for priority_queue
        // has to make a copy.
        // The worker thread will steal ownership from the priority_queue when it runs the function.
        const auto packaged_task = std::make_shared<std::packaged_task<void()>>(
            std::bind(std::forward<Fn>(function), std::forward<Args>(args)...)
        );

        std::unique_lock<std::mutex> lock(_job_queue_mutex);
        _job_queue.emplace(priority, packaged_task);

        return packaged_task->get_future();
    }

    //! Add a new job to the thread pool
    //! \param function Function to call in the thread pool.
    //! \param args Optional arguments to pass to the function.
    //! \return Future for function.
    template<typename Fn, typename... Args> requires (std::is_null_pointer_v<Fn> == false)
    std::future<void> emplace(Fn&& function, Args&&... args) {
        return emplace(DEFAULT_PRIORITY, std::forward<Fn>(function), std::forward<Args>(args)...);
    }

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