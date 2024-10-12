#pragma once

#include <iostream>

#include <dep_fmt/fmt.hpp>

#include <module_logger/types.hpp>
#include <module_logger/handler/LogHandler.hpp>

namespace lib::logger
{
class Std_Handler final : public LogHandler {
public:
    explicit Std_Handler(log_level accepted_level)
        : _accepted_level(accepted_level) {
    }

    void print(const log_message_t& message) final {
        if (message.level > _accepted_level) {
            return;
        }

        // Logs should look like the following
        // | THREAD_ID | %Y-%m-%d %H:%M:%S | LEVEL | [TAG]  | message
        std::cout
                << fmt::format(
                    "| {} | {} | {} | [{}] | {}\n",
                    message.thread_id._Get_underlying_id(),
                    message.timestamp,
                    get_level_identifier(message.level),
                    message.tag,
                    message.message
                );
    }

    void flush() final {
        std::cout << std::flush;
    }

private:
    //! Retrieve the character representation of each log level.
    static std::string_view get_level_identifier(log_level log_level) {
        switch (log_level) {
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

private:
    log_level _accepted_level;

};
}
