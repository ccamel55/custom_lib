#pragma once

// Stringify
#define LIB_STR(a) #a

// Create custom relative include
#define LIB_TO_INCLUDE_STR(a, b) LIB_STR(a ## _ ## b.hpp)
#define LIB_INCLUDE(file, suffix) LIB_TO_INCLUDE_STR(file, suffix)