#pragma once

#include <iostream>
#include <fmt/format.h>

#if DEF_LIB_COMMON_LOGGER != 0

namespace
{
	constexpr auto COLOR_RESET = "\033[0m";
	constexpr auto COLOR_RED = "\033[31m";
	constexpr auto COLOR_YELLOW = "\033[33m";
	constexpr auto COLOR_LIGHT_GRAY = "\033[37m";
	constexpr auto COLOR_WHITE = "\033[39m";
}

#endif

#if DEF_LIB_COMMON_LOGGER >= 4
#define lib_log_d(x) std::cout << COLOR_LIGHT_GRAY << "[debug]: " << x << COLOR_RESET << std::endl
#else
#define lib_log_d(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 3
#define lib_log_i(x) std::cout << COLOR_WHITE << "[info]: " << x << COLOR_RESET << std::endl
#else
#define lib_log_i(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 2
#define lib_log_w(x) std::cout << COLOR_YELLOW << "[warning]: " << x << COLOR_RESET << std::endl
#else
#define lib_log_w(x)
#endif

#if DEF_LIB_COMMON_LOGGER >= 1
#define lib_log_e(x) std::cout << COLOR_RED << "[error]: " << x << COLOR_RESET << std::endl
#else
#define lib_log_e(x)
#endif



