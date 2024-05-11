#pragma once

#include <module_logger/types.hpp>

#include <string>

namespace lib::logger
{
//! Interface for all log handlers
class BaseHandler {
public:
    virtual ~BaseHandler() = default;

    virtual void write_to_log(
            log_level log_level,
            const std::string& message
    ) = 0;

};
}