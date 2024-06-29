# I only have support for x86 (64bit) windows ATM.
if (NOT CURRENT_PLATFORM STREQUAL "win32")
	message(STATUS "module_memory has been disable: platform is not supported")
	return()
endif ()

add_module(
	memory
)