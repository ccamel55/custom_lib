add_library(custom_lib "")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(backend)
add_subdirectory(common)
add_subdirectory(frontend)
add_subdirectory(lib)

# register options
set(LIB_BACKEND_RENDER "none" CACHE STRING "Render implementation")
set_property(CACHE LIB_BACKEND_RENDER PROPERTY STRINGS "none" "opengl3" "vulkan")

set(LIB_BACKEND_INPUT "none" CACHE STRING "Input interception implementation")
set_property(CACHE LIB_BACKEND_INPUT PROPERTY STRINGS "none" "linux" "macos" "win32")

set(LIB_BACKEND_WINDOW "none" CACHE STRING "Window creation implementation")
set_property(CACHE LIB_BACKEND_WINDOW PROPERTY STRINGS "none" "linux" "macos" "win32")

# directory stems from here
target_include_directories(custom_lib PUBLIC
        .
        )