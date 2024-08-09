#include <module_threading/ThreadPool.hpp>

#include <algorithm>

using namespace lib::threading;

ThreadPool::ThreadPool(size_t max_threads)
    : _num_threads(std::clamp<size_t>(max_threads, 1, std::max<size_t>(1, std::thread::hardware_concurrency())))
    , _running(true) {

    const auto worker_thread = [&](size_t thread_id) {
        // Hold the function in our worker, so we can unlock the mutex when we process stuff.
        std::function<void()> function = { };

        while (_running) {
            {
                std::unique_lock<std::mutex> lock(_job_queue_mutex);

                // Wait until we recieve something or until we get told to stop
                _threads_update.wait(
                    lock,
                    [&] {
                        return !_job_queue.empty() || !_running;
                    }
                );

                if (!_running) [[unlikely]] {
                    break;
                }

                // Take ownership from the priority_queue, function is marked mutable!
                function = std::move(_job_queue.top().function);
                _job_queue.pop();
            }

            // Call the wrapped function which will signal the promise once it finishes.
            function();
        }
    };

    // Create our worker threads, the thread ID is the index number.
    for (size_t i = 0; i < _num_threads; i++) {
        _threads.emplace_back(worker_thread, i);
    }
}

ThreadPool::~ThreadPool() {
    // Tell all threads, bedtime!
    if (_running) {
        _running = false;
        _threads_update.notify_all();
    }

    // Wait for all threads to finish before destroying it's self.
    for (auto& thread: _threads) {
        if (!thread.joinable()) {
            continue;
        }

        thread.join();
    }
}

void ThreadPool::clear() {
    std::unique_lock<std::mutex> lock(_job_queue_mutex);
    _job_queue = { };
}

size_t ThreadPool::size() const {
    std::unique_lock<std::mutex> lock(_job_queue_mutex);
    return _job_queue.size();
}

bool ThreadPool::empty() const {
    std::unique_lock<std::mutex> lock(_job_queue_mutex);
    return _job_queue.empty();
}
