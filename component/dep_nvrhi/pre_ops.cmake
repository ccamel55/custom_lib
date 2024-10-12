find_package(Vulkan REQUIRED)

if (NOT ${Vulkan_FOUND})
	message(FATAL_ERROR "Could not find vulkan sdk")
endif ()

CPMAddPackage(
	NAME
		nvrhi
	GITHUB_REPOSITORY
		ccamel55/nvrhi
	GIT_TAG
		main
	OPTIONS
		"NVRHI_BUILD_SHARED OFF"
		"NVRHI_INSTALL OFF"
		"NVRHI_WITH_VULKAN ON"
		"NVRHI_WITH_DX11 OFF"
		"NVRHI_WITH_DX12 OFF"
)