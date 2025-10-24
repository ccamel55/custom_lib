#
# Set compiler and linker flags
#
string(APPEND CMAKE_CXX_FLAGS " -W3 -fp:fast -flto -Oi")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O2 -Oy -Gw -Gy")
string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -Od")
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -Od")