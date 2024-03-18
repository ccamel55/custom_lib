#pragma once
#include <format>

#include <core_sdk/templates/singleton.hpp>
#include <core_sdk/logger/handlers/base_log_handler.hpp>

#include <fmt/format.h>

#include <memory>
#include <vector>

#if DEF_LIB_LOGGER >= 6
#define lib_log_v(x, ...) lib::logger::get().print(lib::log_level::verbose, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_v(x, ...)
#endif

#if DEF_LIB_LOGGER >= 5
#define lib_log_d(x, ...) lib::logger::get().print(lib::log_level::debug, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_d(x, ...)
#endif

#if DEF_LIB_LOGGER >= 4
#define lib_log_i(x, ...) lib::logger::get().print(lib::log_level::info, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_i(x, ...)
#endif

#if DEF_LIB_LOGGER >= 3
#define lib_log_w(x, ...) lib::logger::get().print(lib::log_level::warning, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_w(x, ...)
#endif

#if DEF_LIB_LOGGER >= 2
#define lib_log_e(x, ...) lib::logger::get().print(lib::log_level::error, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_e(x, ...)
#endif

#if DEF_LIB_LOGGER >= 1
#define lib_log_ex(x, ...) lib::logger::get().print(lib::log_level::explcit, fmt::format(x __VA_OPT__(, ) __VA_ARGS__))
#else
#define lib_log_e(x, ...)
#endif

namespace lib
{
//! Lightweight logging class inspired by Timber for Java
class logger: public singleton<logger>
{
public:
    //! Plant a new log handler which will process incoming logs.
    static void plant(const std::shared_ptr<base_log_handler>& handler);

    //! Remove a log handler.
    static void uproot(const std::shared_ptr<base_log_handler>& handler);

    //! Remove all log handlers.
    static void uproot_all();

    //! Print log at specific log level. This API should not be used directly, insteal use the macros define.
    void print(log_level log_level, const std::string& message) const;

private:
    std::vector<std::shared_ptr<base_log_handler>> _handlers = {};

};
}
