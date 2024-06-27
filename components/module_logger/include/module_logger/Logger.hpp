#pragma once

#include <module_core/Singleton.hpp>
#include <module_logger/types.hpp>

#include <dep_fmt/fmt.hpp>

namespace lib::logger {
    //! Lightweight logging interface, with minimal overhead.
    template<logger_impl T>
    class Logger final : public Singleton<Logger<T>> {
    public:
        //! Explicitly send the impl a flush command.
        void flush() {
            _impl.flush();
        }

        //! Write a string log.
        //! \param level Log level.
        //! \param tag Message tag. A human and machine readable identifier/grouping.
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void log(log_level level, const std::string& tag, const fmt::format_string<Args...>& message, Args&&... args) {
            log_message_t message_object = { }; {
                message_object.timestamp = std::chrono::system_clock::now();
                message_object.level     = level;
                message_object.tag       = tag;
                message_object.message   = fmt::format(message, std::forward<Args>(args)...);
            }
            log(message_object);
        }

        void log(const log_message_t& message) {
            _impl.log(message);
        }

    private:
        T _impl = { };

    };
}
