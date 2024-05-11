#pragma once

namespace lib::logger
{
    enum class log_level {
        EXPLICIT,
        ERROR,
        WARNING,
        INFO,
        DEBUG,
        VERBOSE
    };
}