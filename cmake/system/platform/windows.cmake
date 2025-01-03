#
# Windows specific options
#
target_compile_definitions(camel_defines INTERFACE NOMINMAX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")