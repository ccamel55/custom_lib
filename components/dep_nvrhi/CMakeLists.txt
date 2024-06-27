set(NVRHI_BUILD_SHARED OFF)
set(NVRHI_INSTALL ${LIB_INSTALL_DEPENDENCIES})

# todo: support toggle to DX11 and DX12
set(NVRHI_WITH_VULKAN ON)
set(NVRHI_WITH_DX11 OFF)
set(NVRHI_WITH_DX12 OFF)

add_dependency(
	nvrhi
	GITHUB_REPOSITORY
		ccamel55/nvrhi
	GIT_TAG
		main
	DEPENDENCIES
		nvrhi
		nvrhi_vk
)

# We didn't make this library so i don't care about the warnings.
set(NVRHI_COMPILE_FLAGS "-Wno-unused-private-field -Wno-unused-command-line-argument -Wno-deprecated-copy")

set_target_properties(nvrhi PROPERTIES
	COMPILE_FLAGS ${NVRHI_COMPILE_FLAGS}
)

set_target_properties(nvrhi_vk PROPERTIES
	COMPILE_FLAGS ${NVRHI_COMPILE_FLAGS}
)


