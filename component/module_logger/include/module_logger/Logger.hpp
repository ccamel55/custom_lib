#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <dep_fmt/fmt.hpp>

#include <module_logger/types.hpp>
#include <module_logger/handler/LogHandler.hpp>

namespace lib::logger
{
class ScopeLog;

//! Threadsafe logging class
class Logger {
    friend class ScopeLog;

public:
    //! Add a new handler
    //! \param handler Log handler object
    void add_handler(std::shared_ptr<LogHandler> handler) {
        std::unique_lock<std::mutex> lock(_handler_mutex);
        _handler.emplace_back(std::move(handler));
    }

    //! Try to remove a handler
    //! \param handler Log handler to remove
    void remove_handler(const std::shared_ptr<LogHandler>& handler) {
        std::unique_lock<std::mutex> lock(_handler_mutex);
        std::erase_if(
            _handler,
            [&](const auto& x) {
                return x == handler;
            }
        );
    }

    //! Write a string log.
    //! \param level Log level.
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void print(log_level level, const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        log_message_t message_object = { }; {
            message_object.thread_id = std::this_thread::get_id();
            message_object.timestamp = std::chrono::system_clock::now();
            message_object.level     = level;
            message_object.tag       = tag;
            message_object.message   = fmt::format(message, std::forward<Args>(args)...);
        }
        print(message_object);
    }

    //! Write a log of type ERROR
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void e(const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        print(log_level::ERROR, tag, message, std::forward<Args>(args)...);
    }

    //! Write a log of type WARNING
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void w(const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        print(log_level::WARNING, tag, message, std::forward<Args>(args)...);
    }

    //! Write a log of type INFO
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void i(const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        print(log_level::INFO, tag, message, std::forward<Args>(args)...);
    }

    //! Write a log of type DEBUG
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void d(const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        print(log_level::DEBUG, tag, message, std::forward<Args>(args)...);
    }

    //! Write a log of type VERBOSE
    //! \param tag Message tag. A human and machine readable identifier/grouping.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void v(const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
        print(log_level::VERBOSE, tag, message, std::forward<Args>(args)...);
    }

    //! Explicitly flush all handlers
    void flush() {
        std::unique_lock<std::mutex> lock(_handler_mutex);
        for (const auto& handler: _handler) {
            handler->flush();
        }
    }

private:
    //! Write a string log.
    //! \param message_object log message.
    void print(const log_message_t& message_object) {
        // Todo: pass lock etc to handler/do handling on another thread
        std::unique_lock<std::mutex> lock(_handler_mutex);
        for (const auto& handler: _handler) {
            handler->print(message_object);
        }
    }

private:
    std::mutex _handler_mutex                           = {};
    std::vector<std::shared_ptr<LogHandler>> _handler   = {};

};
}
