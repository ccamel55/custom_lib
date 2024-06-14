#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <vector>

namespace lib::threading {
enum class signal_invoke_mode {
    //! Blocking mode blocks the calling thread until all listeners have finished executing.
    BLOCKING,

    //! Async mode does not block but executes the listeners asynchronously
    ASYNC,
};

//! Safely call functions from different threads.
template<typename... Args>
class SignalHandler {
    using listener_id = uint32_t;
    using listener_fn = std::function<void(Args...)>;

    struct listener_object_t {
        listener_object_t(listener_id id, listener_fn&& fn)
            : id(id)
            , fn(std::move(fn)) {
        }

        listener_id id;
        listener_fn fn;
    };

public:
    explicit SignalHandler(signal_invoke_mode mode = signal_invoke_mode::BLOCKING)
        : _invoke_mode(mode)
        , _id_counter(0) {
    }

    ~SignalHandler() {
        // Wait until we get lock otherwise we risk nuking this object while things are still referencing it.
        std::unique_lock<std::mutex> lock(_vector_mutex);
    }

    //! Invoke all registered listeners.
    //! \param args Arguments to pass onto the invokable.
    void invoke(Args... args) {
        if (empty()) {
            return;
        }

        const auto call_listeners = [&] {
            // We lock for the duration of the call because we don't want to introduce new
            // listeners mid-call.
            std::unique_lock<std::mutex> lock(_vector_mutex);

            for (const auto& listener: _listeners) {
                listener.fn(args...);
            }
        };

        if (_invoke_mode == signal_invoke_mode::BLOCKING) {
            call_listeners();
        }
    }

    //! Add a new listener, guaranteeing that the listener will be added.
    //! \param listener Listener that will be registered.
    //! \return Unique ID that is used to identify a listener.
    listener_id emplace(listener_fn&& listener) {
        std::unique_lock<std::mutex> lock(_vector_mutex);

        const auto id = _id_counter++;
        _listeners.emplace_back(id, std::move(listener));

        return id;
    }

    //! Add a new listener if the caller can obtain a lock.
    //! \param listener Listener that will be registered.
    //! \return Unique ID that is used to identify a listener if listener was registered successfully
    //! \return std::nullopt if thread can not obtain a lock.
    std::optional<listener_id> try_emplace(listener_fn&& listener) {
        std::unique_lock<std::mutex> lock(_vector_mutex, std::try_to_lock);

        if (!lock.owns_lock()) {
            return std::nullopt;
        }

        const auto id = _id_counter++;
        _listeners.emplace_back(id, std::move(listener));

        return id;
    }

    //! Remove a listener, guaranteeing that the listener will be removed.
    //! \param id The identifier for a specific listener.
    void erase(listener_id id) {
        std::unique_lock<std::mutex> lock(_vector_mutex);
        std::erase_if(
            _listeners,
            [&](const auto& x) {
                return x.id == id;
            }
        );
    }

    //! Remove a listener, if the caller can obtain a lock.
    //! \param id The identifier for a specific listener.
    //! \return true if the caller tried to remove the listener
    //! \return false if the caller could not obtain a lock
    bool try_erase(listener_id id) {
        std::unique_lock<std::mutex> lock(_vector_mutex, std::try_to_lock);

        if (!lock.owns_lock()) {
            return false;
        }

        std::erase_if(
            _listeners,
            [&](const auto& x) {
                return x.id == id;
            }
        );

        return true;
    }

    //! Remove all listeners.
    void clear() {
        std::unique_lock<std::mutex> lock(_vector_mutex);
        _listeners.clear();
    }

    //! Number of listeners registered.
    [[nodiscard]] size_t size() const {
        std::unique_lock<std::mutex> lock(_vector_mutex);
        return _listeners.size();
    }

    //! Whether there are listeners or not.
    [[nodiscard]] bool empty() const {
        std::unique_lock<std::mutex> lock(_vector_mutex);
        return _listeners.empty();
    }

private:
    signal_invoke_mode _invoke_mode = signal_invoke_mode::BLOCKING;
    uint32_t _id_counter            = 0;

    mutable std::mutex _vector_mutex          = { };
    std::vector<listener_object_t> _listeners = { };

};
}
