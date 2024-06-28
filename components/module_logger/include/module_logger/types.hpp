#pragma once

#include <chrono>
#include <string>
#include <thread>

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
        std::thread::id thread_id;
        std::chrono::system_clock::time_point timestamp;
        log_level level;
        std::string tag;
        std::string message;
    };
}
