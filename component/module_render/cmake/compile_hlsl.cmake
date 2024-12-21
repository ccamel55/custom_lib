# todo: change this, we currently use VULKAN_SDK env var to get correct DXC binary
if (CAMEL_PLATFORM_WINDOWS)
	set(_VULKAN_DXC_PATH "$ENV{VULKAN_SDK}/bin/dxc.exe" CACHE INTERNAL "")
else ()
	if (NOT LINUX_DXC_PATH AND NOT DEFINED ENV{LINUX_DXC_PATH})
		message(FATAL_ERROR "LINUX_DXC_PATH is not set. This must be the absolute path of dxc binary when building for linux.")
	endif ()

	if (DEFINED ENV{LINUX_DXC_PATH})
	   set(LINUX_DXC_PATH "$ENV{LINUX_DXC_PATH}")
	endif ()

	set(_VULKAN_DXC_PATH "${LINUX_DXC_PATH}" CACHE INTERNAL "")
endif ()

if (NOT EXISTS ${_VULKAN_DXC_PATH})
	message(FATAL_ERROR "Could not find DXC")
endif ()

if (NOT ${Python3_FOUND})
	message(FATAL_ERROR "Could not find python 3")
endif ()

# This needs to be set when the CMake script file is evaluated otherwise we will have incorrect path
# when calling this from different `CMakeLists.txt
set(_SHADER_COMPILE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/support/compile_vulkan_shader.py" CACHE INTERNAL "")

# Shader main output location
set(_SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shader_spv CACHE INTERNAL "")

# Make sure output location exists
file(MAKE_DIRECTORY ${_SHADER_OUTPUT_DIR})


# Build shaders from an input directory containing source hlsl code.
macro(compile_shaders TARGET INPUT)
	project("hlsl_${TARGET}")

	add_custom_target(
		${PROJECT_NAME} ALL
		COMMAND
			${Python3_EXECUTABLE} ${_SHADER_COMPILE_SCRIPT}
				-e ${_VULKAN_DXC_PATH}
				-i ${INPUT}
				-o ${_SHADER_OUTPUT_DIR}
				-vk # We will always build in vulkan mode for now
		COMMENT
			"Compiling HLSL shaders (${TARGET})"
	)

	# Make sure we call this when building a specific target
	add_dependencies(${TARGET} ${PROJECT_NAME})
endmacro()


# Copy shaders from default build directory into another directory
macro(copy_shaders TARGET OUTPUT)
	add_custom_command(
		TARGET
			${TARGET} POST_BUILD
		COMMAND
			${CMAKE_COMMAND} -E copy_directory
				${_SHADER_OUTPUT_DIR}
				${OUTPUT}
	)

	# Update install with shaders
	install(
		DIRECTORY ${OUTPUT}
		TYPE BIN
		TYPE LIB
	)
endmacro()