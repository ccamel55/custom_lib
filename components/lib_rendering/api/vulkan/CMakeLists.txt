# find additional dependencies
find_package(Vulkan REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT ${Vulkan_FOUND})
    message(FATAL_ERROR "Could not find vulkan sdk")
else ()
    message(STATUS "Vulkan: Include directories: ${Vulkan_INCLUDE_DIRS}")
    message(STATUS "Vulkan: Libraries directories: ${Vulkan_LIBRARIES}")
endif ()

if (NOT ${Python3_FOUND})
    message(FATAL_ERROR "Could not find python 3")
else ()
    message(STATUS "Python 3: executable: ${Python3_EXECUTABLE}")
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

# setup pre build python script
add_custom_target(
    compile_shaders ALL
    COMMAND
        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/python/compile_vulkan_shaders.py
        -i ${CMAKE_CURRENT_LIST_DIR}/shaders
        -o ${CMAKE_CURRENT_LIST_DIR}/include/lib_rendering/shaders
        -n lib::rendering::vulkan::shaders
    COMMENT
        "Compiling vulkan shaders."
)

add_dependencies(lib_rendering compile_shaders)