lib_option(CAMEL_NVRHI_VULKAN "Enable Vulkan support" ON)
lib_option(CAMEL_NVRHI_DX_11 "Enable DirectX 11 support" OFF)
lib_option(CAMEL_NVRHI_DX_12 "Enable DirectX 12 support" OFF)

if (CAMEL_NVRHI_VULKAN)
	find_package(Vulkan REQUIRED)

	if (NOT ${Vulkan_FOUND})
		message(FATAL_ERROR "Could not find vulkan sdk")
	endif ()
endif()

if (CAMEL_NVRHI_DX_11)
	message(FATAL_ERROR "DirectX 11 is not supported yet.")
endif()

if (CAMEL_NVRHI_DX_12)
	message(FATAL_ERROR "DirectX 12 is not supported yet.")
endif()

CPMAddPackage(
	NAME
		nvrhi
	GITHUB_REPOSITORY
		NVIDIAGameWorks/nvrhi
	GIT_TAG
		main
	OPTIONS
		"NVRHI_BUILD_SHARED OFF"
		"NVRHI_INSTALL OFF"
		"NVRHI_WITH_VULKAN ${CAMEL_NVRHI_VULKAN}"
		"NVRHI_WITH_DX11 ${CAMEL_NVRHI_DX_11}"
		"NVRHI_WITH_DX12 ${CAMEL_NVRHI_DX_12}"
)