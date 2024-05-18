#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace lib::threading
{
class ThreadPool {
    using queue_object = std::pair<uint8_t, std::function<void()>>;

    // Custom comparison function for std::priority_queue
    struct min_priority_t {
        bool operator()(const queue_object& l, const queue_object& r) const {
            return l.first > r.first;
        }
    };

public:
    //! Create a new thread pool object.
    //! \param max_threads The maximum number of threads to use in the thread pool. Defaults to 0.
    explicit ThreadPool(size_t max_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    //! Add a new job to the thread pool
    void emplace();

    //! Add a new job to the thread pool and return a future for that job.
    void emplace_future();

    //! Remove all queued jobs from the thread pool.
    void clear();

    //! Returns the number of jobs queued in the thread pool.
    [[nodiscard]] size_t size() const;

    //! Whether the thread pool is empty or not.
    [[nodiscard]] bool empty() const;

private:
    size_t _num_threads = 0;
    std::atomic<bool> _running = false;

    std::condition_variable _threads_update = {};
    std::vector<std::thread> _threads = {};

    std::mutex _job_queue_mutex = {};
    std::priority_queue<queue_object, std::deque<queue_object>, min_priority_t> _job_queue = {};

};
}