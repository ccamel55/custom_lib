#include <module_threading/ThreadPool.hpp>

using namespace lib::threading;

ThreadPool::ThreadPool(size_t max_threads)
    : _num_threads(std::clamp<size_t>(max_threads, 1, std::max<size_t>(1, std::thread::hardware_concurrency())))
    , _running(true) {

    const auto worker_thread = [&](size_t thread_id){

        // Hold the function in our worker so we can unlock the mutex when we process stuff.
        std::function<void()> function = nullptr;

        while(true) {
            std::unique_lock<std::mutex> lock(_job_queue_mutex);

            // If empty, wait until we receive something otherwise keep going, eat it up.
            if (_job_queue.empty()) {
                _threads_update.wait(lock);
            }

            if (!_running) [[unlikely]] {
                break;
            }

            // todo: pop from priority queue, then run
        }
    };

    // Create our worker threads, the thread ID is the index number.
    for (size_t i = 0; i < _num_threads; i++) {
        _threads.emplace_back(worker_thread, i);
    }
}

ThreadPool::~ThreadPool() {
    // Make sure we hold this mutex otherwise we risk destroying the vector while another thread is accessing it.
    std::unique_lock<std::mutex> lock(_job_queue_mutex);

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