#pragma once

#include <atomic>
#include <memory>

namespace lib::threading
{
//! Copy and lock free triple buffer.
template <typename T>
class TripleBuffer {
public:
    TripleBuffer(
        const std::shared_ptr<T>& read_buffer,
        const std::shared_ptr<T>& write_buffer,
        const std::shared_ptr<T>& ready_buffer
    )   : _reading(read_buffer)
        , _writing(write_buffer)
        , _ready(ready_buffer) {
        // dont swap by default
        _updated.exchange(true);
    }

    //! Get the current write buffer.
    //! \return A std::shared_ptr of the write buffer.
    [[nodiscard]] const std::shared_ptr<T>& get_write_buffer() const {
        return _writing.load();
    }

    //! Mark end of write.
    void finish_write() {
        // swap ready and write buffers
        auto p = _ready.exchange(_writing);
        _writing.store(p);

        // mark updated info
        _updated.exchange(false);
    }

    //! Get the current read buffer.
    //! \return A std::shared_ptr of the read buffer.
    [[nodiscard]] const std::shared_ptr<T>& get_read_buffer() const {
        return _reading.load();
    }

    //! Mark end of read.
    void finish_read() {
        // if we didn't update then dont swap
        if (_updated.exchange(true)) {
            return;
        }

        // swap only if we updated
        auto p = _ready.exchange(_reading);
        _reading.store(p);
    }

private:
    std::atomic<std::shared_ptr<T>> _reading = nullptr;
    std::atomic<std::shared_ptr<T>> _writing = nullptr;
    std::atomic<std::shared_ptr<T>> _ready = nullptr;

    std::atomic<bool> _updated = false;

};
}  // namespace lib::threading