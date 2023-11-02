#pragma once
#include <fmt/format.h>
#include <fmt/color.h>

#if DEF_LIB_COMMON_LOGGER >= 4
#define lib_log_d(x, args...) fmt::print(fg(fmt::color::gray), "(D): " x "\n", args)
#else
#define lib_log_d(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 3
#define lib_log_i(x, args...) fmt::print(fg(fmt::color::white),"(I): " x "\n", args)
#else
#define lib_log_i(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 2
#define lib_log_w(x, args...) fmt::print(fg(fmt::color::yellow),"(W): " x "\n", args)
#else
#define lib_log_w(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 1
#define lib_log_e(x, args...) fmt::print(fg(fmt::color::red),"(E): " x "\n", args)
#else
#define lib_log_e(x)
#endif
