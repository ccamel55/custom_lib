message(STATUS "Configuring: ${PROJECT_NAME} - ${PROJECT_DESCRIPTION}")

function(cpm_message)
	# Override `cpm_message` so `cpm` doesn't output garbage
endfunction()

include(cpm)
include(defines)

if (_CAMEL_IS_TOP_LEVEL)
	include(option/main)
endif ()

include(system/main)

if (_CAMEL_IS_TOP_LEVEL)
	include(dependency/main)
	include(module/main)

	scan_components()
	add_components()

	scan_tools()
	add_tools()
endif ()

message(STATUS ${_CAMEL_CMAKE_SPACER})