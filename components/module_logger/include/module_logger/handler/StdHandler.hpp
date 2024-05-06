#pragma once
#include <module_logger/BaseHandler.hpp>

namespace lib
{
class StdHandler : public BaseHandler {
public:
    void write_to_log(
            log_level log_level,
            const std::string& message
    ) override;

};
}
