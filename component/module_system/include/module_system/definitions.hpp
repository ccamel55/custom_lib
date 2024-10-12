#pragma once

#ifdef __APPLE__
#define PLATFORM_APPLE
#error "Todo: support Mac OS, it should be similar to Linux."
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define PLATFORM_LINUX
#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
#define PLATFORM_WIN32
#else
#error "Platform not supported. Eat poo."
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define ARCH_x86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define ARCH_x86
#elif defined(_M_ARM64) || defined(__aarch64__)
#define ARCH_ARM64
#endif
