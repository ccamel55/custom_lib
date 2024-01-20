# find vulkan
find_package(Vulkan REQUIRED)

if (NOT ${Vulkan_FOUND})
    message(FATAL_ERROR "Could not find vulkan sdk")
else ()
    message(STATUS "Vulkan: Include directories: ${Vulkan_INCLUDE_DIRS}")
    message(STATUS "Vulkan: Libraries directories: ${Vulkan_LIBRARIES}")
endif ()

lib_sub_component(
    PARENT
        lib_rendering
    SOURCES
        render_api.cpp
    INCLUDES
        include
        ${Vulkan_INCLUDE_DIRS}
    LINK_LIBS
        ${Vulkan_LIBRARIES}
)