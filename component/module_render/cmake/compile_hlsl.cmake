# todo: change this, we currently use VULKAN_SDK env var to get correct DXC binary
if (CAMEL_PLATFORM_WINDOWS)
	set(_VULKAN_DXC_PATH "$ENV{VULKAN_SDK}/bin/dxc.exe" CACHE INTERNAL "")
else ()
	if (NOT LINUX_DXC_PATH)
		message(FATAL_ERROR "LINUX_DXC_PATH is not set. This must be the absolute path of dxc binary when building for linux.")
	endif ()

	set(_VULKAN_DXC_PATH "${LINUX_DXC_PATH}" CACHE INTERNAL "")
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
	set(
		_ARG_DEF
		"	"
	)

	# Template - one value arguments
	set(
		_ARG_ONE
			INPUT
			OUTPUT
			NAMESPACE
	)

	# Template - multi value arguments
	set (
		_ARG_MULTI
			""
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
			${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/support/compile_vulkan_shader.py
				-e ${_VULKAN_DXC_PATH}
				-i ${${PROJECT_NAME}_INPUT}
				-o ${${PROJECT_NAME}_OUTPUT}
				-n ${${PROJECT_NAME}_NAMESPACE}
				-vk # We will always build in vulkan mode for now
		COMMENT
			"Compiling HLSL shaders (${TARGET})"
	)

	# Make sure we call this when building a specific target
	add_dependencies(${TARGET} ${PROJECT_NAME})
endmacro()