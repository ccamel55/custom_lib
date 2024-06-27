# I only have support for x86 (64bit) windows ATM.
set(_ENABLE_MODULE_MEMORY OFF)

if (CURRENT_ARCH STREQUAL "x86" AND CURRENT_PLATFORM STREQUAL "win32")
	set(_ENABLE_MODULE_MEMORY ON)
else ()
	set(_ENABLE_MODULE_MEMORY OFF)
endif ()

add_module(
	memory
	REQUIRES
		_ENABLE_MODULE_MEMORY
)