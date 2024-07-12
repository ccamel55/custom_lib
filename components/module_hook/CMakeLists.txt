# I only have support for x86 (64bit) windows ATM.
if (NOT CURRENT_PLATFORM STREQUAL "win32")
	return()
endif ()

add_module(
	hook
	DEPENDENCIES
		dep_zasm
		dep_zydis
		module_memory
)