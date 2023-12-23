#pragma once

#include <fmt/color.h>
#include <fmt/format.h>

#if DEF_LIB_LOGGER >= 4
#define lib_log_d(x, ...) fmt::print(fg(fmt::color::gray), "(D): " x "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
#define lib_log_d(x, args...)
#endif

#if DEF_LIB_LOGGER >= 3
#define lib_log_i(x, ...) fmt::print(fg(fmt::color::white), "(I): " x "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
#define lib_log_i(x, args...)
#endif

#if DEF_LIB_LOGGER >= 2
#define lib_log_w(x, ...) fmt::print(fg(fmt::color::yellow), "(W): " x "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
#define lib_log_w(x, args...)
#endif

#if DEF_LIB_LOGGER >= 1
#define lib_log_e(x, ...) fmt::print(fg(fmt::color::red), "(E): " x "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
#define lib_log_e(x, args...)
#endif
