#pragma once

#include <module_logger/types.hpp>

#include <string>

namespace lib::logger
{
//! Interface for all log handlers
class Base_Handler {
public:
    virtual ~Base_Handler() = default;

    virtual void write_to_log(
            log_level log_level,
            const std::string& message
    ) = 0;

};
}