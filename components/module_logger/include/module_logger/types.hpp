#pragma once

#include <chrono>
#include <string>

namespace lib::logger {
    enum class log_level {
        EXPLICIT,
        ERROR,
        WARNING,
        INFO,
        DEBUG,
        VERBOSE
    };

    struct log_message_t {
        std::chrono::system_clock::time_point timestamp;
        log_level level;
        std::string tag;
        std::string message;
    };

    template<typename T>
    concept logger_impl = requires(T& t, const log_message_t& message) {
        { t.log(message) };
        { t.flush() };
    };
}
