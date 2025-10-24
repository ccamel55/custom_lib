#
# Set compiler and linker flags
#
string(APPEND CMAKE_CXX_FLAGS " -Wall -ffast-math -ftree-vectorize -flto=auto -march=native")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -Ofast -fomit-frame-pointer")
string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -Og")
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -Og")