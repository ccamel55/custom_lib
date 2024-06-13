#pragma once

#include <iostream>

#include <dep_fmt/fmt.hpp>

#include <module_logger/types.hpp>

namespace lib::logger
{
class Std_Handler {
public:
    void log(const log_message_t& message) {
        const auto get_level_identifier = [&]{
            switch (message.level)
            {
            case log_level::EXPLICIT:
                return "EX";
            case log_level::ERROR:
                return "E";
            case log_level::WARNING:
                return "W";
            case log_level::INFO:
                return "I";
            case log_level::DEBUG:
                return "D";
            case log_level::VERBOSE:
                return "V";
            }

            // This should never be reached
            return "V";
        };

        // Logs should look like the following
        // | %Y-%m-%d %H:%M:%S | LEVEL | [TAG]  | message
        std::cout
            << fmt::format(
                "| {} | {} | [{}] | {}\n",
                message.timestamp,
                get_level_identifier(),
                message.tag,
                message.message
            );
    }

    void flush() {
        std::cout << std::flush;
    }

private:

};
}
