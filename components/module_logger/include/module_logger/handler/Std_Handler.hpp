#pragma once
#include <module_logger/handler/Base_Handler.hpp>

namespace lib::logger
{
class Std_Handler : public Base_Handler {
public:
    void write_to_log(
            log_level log_level,
            const std::string& message
    ) override;

};
}
