#pragma once
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
#define lib_print_d(x) std::cout << COLOR_LIGHT_GRAY << "[debug]: " << x << COLOR_RESET << std::endl;
#else
#define lib_print_d(x);
#endif

#if DEF_LIB_COMMON_LOGGER >= 3
#define lib_print_i(x) std::cout << COLOR_WHITE << "[info]: " << x << COLOR_RESET << std::endl;
#else
#define lib_print_i(x);
#endif

#if DEF_LIB_COMMON_LOGGER >= 2
#define lib_print_w(x) std::cout << COLOR_YELLOW << "[warning]: " << x << COLOR_RESET << std::endl;
#else
#define lib_print_w(x);
#endif

#if DEF_LIB_COMMON_LOGGER >= 1
#define lib_print_e(x) std::cout << COLOR_RED << "[error]: " << x << COLOR_RESET << std::endl;
#else
#define lib_print_e(x);
#endif



