#pragma once
#include <format>

#include <core_sdk/templates/singleton.hpp>
#include <core_sdk/logger/log_handler.hpp>

#include <fmt/format.h>

#include <memory>
#include <vector>

namespace lib
{
//! Lightweight logging class inspired by Timber for Java
class logger: singleton<logger> // dont public the singleton, this is explict
{
public:
    //! Plant a new log handler which will process incoming logs.
    static void plant(const std::shared_ptr<log_handler>& handler);

    //! Remove a log handler.
    static void uproot(const std::shared_ptr<log_handler>& handler);

    //! Remove all log handlers.
    static void uproot_all();

    template<typename... T>
    static void d(fmt::format_string<T...> fmt, T&&... args)
    {
        get().print(log_level::debug, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

    template<typename... T>
    static void i(fmt::format_string<T...> fmt, T&&... args)
    {
        get().print(log_level::info, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

    template<typename... T>
    static void w(fmt::format_string<T...> fmt, T&&... args)
    {
        get().print(log_level::warning, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

    template<typename... T>
    static void e(fmt::format_string<T...> fmt, T&&... args)
    {
        get().print(log_level::error, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

private:
    void print(
        log_level log_level,
        const std::string& message) const
    {
        for (const auto& handler: _handlers)
        {
            handler->write_to_log(log_level, message);
        }
    }

    std::vector<std::shared_ptr<log_handler>> _handlers = {};
};
}
