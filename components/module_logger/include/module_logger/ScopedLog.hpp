#pragma once

#include <module_logger/Logger.hpp>

namespace lib::logger {
//! Log manager than handles logs in the current scope.
template<logger_impl T>
class ScopeLog {
public:
    //! \param tag The tag for all logs within this scope.
    //! \param flush_when_unscoped Whether or not to explicitly flush when leaving scope.
    explicit ScopeLog(const std::string_view& tag, bool flush_when_unscoped = false)
        : _logger(Logger<T>::get())
        , _tag(tag)
        , _flush_when_unscoped(flush_when_unscoped) {

    }

    ~ScopeLog() {
        if (!_flush_when_unscoped) {
            return;
        }

        _logger.flush();
    }

    //! Write a string log.
    //! \param level Log level.
    //! \param message String containing message body.
    //! \param args Packed arguments list containing things to be formatted.
    template<typename... Args>
    void log(log_level level, const fmt::format_string<Args...>& message, Args&&... args) {
        log_message_t message_object = { }; {
            message_object.timestamp = std::chrono::system_clock::now();
            message_object.level     = level;
            message_object.tag       = _tag;
            message_object.message   = fmt::format(message, std::forward<Args>(args)...);
        }
        _logger.log(message_object);
    }

private:
    Logger<T>& _logger;

    std::string_view _tag;
    bool _flush_when_unscoped = false;

};
}
