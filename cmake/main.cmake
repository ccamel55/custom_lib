message(STATUS "Configuring: ${PROJECT_NAME} - ${PROJECT_DESCRIPTION}")

function(cpm_message)
	# Override `cpm_message` so `cpm` doesn't output garbage
endfunction()

include(cpm)
include(defines)

include(option/main)
include(dependency/main)
include(system/main)
include(module/main)

scan_components()
add_components()

scan_tools()
add_tools()

message(STATUS ${_CAMEL_CMAKE_SPACER})