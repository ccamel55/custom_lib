#pragma once

#include <memory>

#include <module_threading/thread_pool/Base_Scheduler.hpp>

namespace lib::threading
{
class ThreadPool {
public:
    //! Create a new thread pool object.
    //! \param scheduler The scheduler used to select next job.
    //! \param max_threads The maximum number of threads to use in the thread pool. Defaults to 0.
    ThreadPool(std::unique_ptr<Base_Scheduler>&& scheduler, size_t max_threads = 0);

    ~ThreadPool();

    //! Start the thread pool by spawning workers.
    void start();

    //! Stop all workers.
    void stop();

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
    size_t _max_threads = 0;
    std::unique_ptr<Base_Scheduler> _scheduler = {};

};
}