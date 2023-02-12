#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace lib::common {

    //! no copy, lock free triple buffer. this should be pretty fast!
    template<class T>
    class triple_buffer {
    public:
        triple_buffer(T& read_buffer, T& write_buffer, T& ready_buffer)
        : _reading(&read_buffer)
        , _writing(&write_buffer)
        , _ready(&ready_buffer) {

            // dont swap by default
            _updated.exchange(true);
        }

        const T* get_write_buffer() const {
            return _writing.load();
        }

        void finish_write() {

            // swap ready and write buffers
            auto p = _ready.exchange(_writing);
            _writing.store(p);

            // mark updated info
            _updated.exchange(false);
        }

        const T* get_read_buffer() const {
            return _reading.load();
        }

        void finish_read() {

            // if we didnt update then dont swap
            if (_updated.exchange(true)) {
                return;
            }

            // swap only if we updated
            auto p = _ready.exchange(_reading);
            _reading.store(p);
        }
    private:
        std::atomic<T*> _reading = nullptr;
        std::atomic<T*> _writing = nullptr;

        std::atomic<bool> _updated = false;
        std::atomic<T*> _ready = nullptr;
    };
}