#include <core_sdk/logger/handlers/stdio_handler.hpp>

#include <fmt/color.h>

#include <iostream>

using namespace lib::log_handler;

namespace
{
    constexpr auto explicit_log_color = fmt::color::blue_violet;
    constexpr auto error_log_color = fmt::color::red;
    constexpr auto warning_log_color = fmt::color::yellow;
    constexpr auto info_log_color = fmt::color::white;
    constexpr auto debug_log_color = fmt::color::light_gray;
    constexpr auto verbose_log_color = fmt::color::light_gray;
}

void stdio_handler::write_to_log(log_level log_level, const std::string& message)
{
    switch (log_level)
    {
        case log_level::explcit:
            fmt::print(fg(explicit_log_color), "(ex)\t" + message + "\n");
            break;
        case log_level::error:
            fmt::print(fg(error_log_color), "(e):\t" + message + "\n");
            break;
        case log_level::warning:
            fmt::print(fg(warning_log_color), "(w):\t" + message + "\n");
            break;
        case log_level::info:
            fmt::print(fg(info_log_color), "(i):\t" + message + "\n");
            break;
        case log_level::debug:
            fmt::print(fg(debug_log_color), "(d):\t" + message + "\n");
            break;
        case log_level::verbose:
            fmt::print(fg(verbose_log_color), "(v):\t" + message + "\n");
            break;
    }
}
