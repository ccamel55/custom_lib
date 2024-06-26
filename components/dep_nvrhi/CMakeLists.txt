set(NVRHI_BUILD_SHARED OFF)
set(NVRHI_INSTALL OFF)

# todo: support toggle to DX11 and DX12
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



