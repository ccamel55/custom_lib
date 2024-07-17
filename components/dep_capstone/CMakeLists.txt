CPMAddPackage(
	NAME
		capstone
	GITHUB_REPOSITORY
		capstone-engine/capstone
	GIT_TAG
		5.0.1
	OPTIONS
		"BUILD_SHARED_LIBS OFF"
		"CAPSTONE_BUILD_STATIC_RUNTIME OFF"
		"CAPSTONE_BUILD_DIET OFF"
		"CAPSTONE_BUILD_TESTS OFF"
		"CAPSTONE_BUILD_CSTOOL OFF"
		"CAPSTONE_BUILD_CSTEST OFF"
		"CAPSTONE_DEBUG OFF"
		"CAPSTONE_INSTALL ${LIB_INSTALL_DEPENDENCIES}"
		"CAPSTONE_X86_ATT_DISABLE ON"
		"CAPSTONE_OSXKERNEL_SUPPORT OFF" # todo: enable when building on macos
)

add_dependency(
	capstone
	DEPENDENCIES
		capstone
)

# We didn't make this library so i don't care about the warnings.
target_compile_options(capstone PRIVATE
	"-Wno-unused-function" "-Wno-unused-variable" "-Wno-unused-value" "-Wno-unused-but-set-variable" "-Wno-deprecated-declarations"
)
