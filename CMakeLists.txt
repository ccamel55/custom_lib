cmake_minimum_required(VERSION 3.26)

project(custom_lib)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Add cmake module paths
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(components/Dependency)
include(components/Module)

include(dependencies/CPM)

message(STATUS "------------------------------------------")
message(STATUS "Custom Lib")

# Apply build options according to CMAKE_BUILD_TYPE
include(Options)
include(CompileFlags)

# Add all components and dependencies as a submodule
include(RegisterComponents)

