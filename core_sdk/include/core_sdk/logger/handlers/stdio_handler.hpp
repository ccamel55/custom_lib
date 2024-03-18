#pragma once

#include <core_sdk/logger/handlers/base_log_handler.hpp>

namespace lib::log_handler
{
class stdio_handler: public base_log_handler
{
public:
    void write_to_log(
        log_level log_level,
        const std::string& message) override;
};
}