#pragma once
#include <string>

namespace lib
{
enum class log_level: uint8_t
{
    explcit,
    error,
    warning,
    info,
    debug,
    verbose,
};

//! Interface for all log handlers
class base_log_handler
{
public:
    virtual ~base_log_handler() = default;

    virtual void write_to_log(
        log_level log_level,
        const std::string& message) = 0;
};
}