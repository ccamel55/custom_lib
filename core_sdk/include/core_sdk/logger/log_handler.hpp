#pragma once
#include <string>

namespace lib
{
enum class log_level: uint8_t
{
    debug,
    info,
    warning,
    error,
};

//! Interface for all log handlers
class log_handler
{
public:
    virtual void write_to_log(
        log_level log_level,
        const std::string& message);
};
}