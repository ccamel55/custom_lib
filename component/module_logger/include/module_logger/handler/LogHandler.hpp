#pragma once

#include <module_logger/types.hpp>

namespace lib::logger
{
class LogHandler {
public:
    //! Print a log message using the handler
    virtual void print(const log_message_t& message) = 0;

    //! Flush/commit any changes made by the handler.
    virtual void flush() = 0;

};
}