find_package(Python3 REQUIRED COMPONENTS Interpreter)

# todo: change this, we currently use VULKAN_SDK env var to get correct DXC binary
if (CURRENT_PLATFORM STREQUAL "win32")
	set(_VULKAN_DXC_PATH "$ENV{VULKAN_SDK}/bin/dxc.exe" CACHE INTERNAL "")
else ()
	set(_VULKAN_DXC_PATH "$ENV{VULKAN_SDK}/bin/dxc" CACHE INTERNAL "")
endif ()

if (NOT EXISTS ${_VULKAN_DXC_PATH})
	message(FATAL_ERROR "Could not find DXC")
endif ()

if (NOT ${Python3_FOUND})
	message(FATAL_ERROR "Could not find python 3")
endif ()

macro(compile_hlsl TARGET)
	project("hlsl_${TARGET}")

	# Template - defined arguments
	set(_ARG_DEF
	)

	# Template - one value arguments
	set(_ARG_ONE
		INPUT
		OUTPUT
		NAMESPACE
	)

	# Template - multi value arguments
	set (_ARG_MULTI
	)

	cmake_parse_arguments(
		${PROJECT_NAME}
		"${_ARG_DEF}"
		"${_ARG_ONE}"
		"${_ARG_MULTI}"
		${ARGN}
	)

	add_custom_target(
		${PROJECT_NAME} ALL
		COMMAND
			${Python3_EXECUTABLE} ${module_render_directory}/support/compile_vulkan_shader.py
			-e ${_VULKAN_DXC_PATH}
			-i ${${PROJECT_NAME}_INPUT}
			-o ${${PROJECT_NAME}_OUTPUT}
			-n ${${PROJECT_NAME}_NAMESPACE}
		COMMENT
			"Compiling HLSL shaders (${TARGET})"
	)

	# Make sure we call this when building a specific target
	add_dependencies(${TARGET} ${PROJECT_NAME})
endmacro()