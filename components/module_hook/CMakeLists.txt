# I only have support for x86 (64bit) ATM.
if (NOT CURRENT_ARCH STREQUAL "x86")
	return()
endif ()

add_module(
	hook
	NO_INSTALL
	DEPENDENCIES
		dep_capstone
		dep_fmt
		module_memory
		module_system
)