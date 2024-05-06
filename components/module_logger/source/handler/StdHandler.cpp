#include <module_logger/handler/StdHandler.hpp>
#include <fmt/color.h>

using namespace lib;

namespace
{
constexpr auto COL_EXPLICIT =   fmt::color::blue_violet;
constexpr auto COL_ERROR =      fmt::color::red;
constexpr auto COL_WARNING =    fmt::color::yellow;
constexpr auto COL_INFO =       fmt::color::white;
constexpr auto COL_DEBUG =      fmt::color::light_gray;
constexpr auto COL_VERBOSE =    fmt::color::light_gray;
}

void StdHandler::write_to_log(log_level log_level, const std::string& message) {
    switch (log_level) {
    case log_level::EXPLICIT:
        fmt::print(fg(COL_EXPLICIT), "(ex)\t" + message + "\n");
        break;
    case log_level::ERROR:
        fmt::print(fg(COL_ERROR), "(e):\t" + message + "\n");
        break;
    case log_level::WARNING:
        fmt::print(fg(COL_WARNING), "(w):\t" + message + "\n");
        break;
    case log_level::INFO:
        fmt::print(fg(COL_INFO), "(i):\t" + message + "\n");
        break;
    case log_level::DEBUG:
        fmt::print(fg(COL_DEBUG), "(d):\t" + message + "\n");
        break;
    case log_level::VERBOSE:
        fmt::print(fg(COL_VERBOSE), "(v):\t" + message + "\n");
        break;
    }
}