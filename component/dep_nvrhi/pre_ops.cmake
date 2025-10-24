lib_option(CAMEL_NVRHI_VULKAN "Enable Vulkan support" ON)
lib_option(CAMEL_NVRHI_DX_11 "Enable DirectX 11 support" OFF)
lib_option(CAMEL_NVRHI_DX_12 "Enable DirectX 12 support" OFF)

set(NVRHI_WITH_VULKAN "${CAMEL_NVRHI_VULKAN}" CACHE BOOL "" FORCE)
set(NVRHI_WITH_DX11 "${CAMEL_NVRHI_DX_11}" CACHE BOOL "" FORCE)
set(NVRHI_WITH_DX12 "${CAMEL_NVRHI_DX_12}" CACHE BOOL "" FORCE)

CPMAddPackage(
	NAME
		nvrhi
	GITHUB_REPOSITORY
		NVIDIAGameWorks/nvrhi
	GIT_TAG
		14cd30e2c5976a3186ace704bcd26d7d9d1736ea
)

# Note: you need to set LINUX_DXC_PATH to DXC on linux platforms.
# download: https://github.com/microsoft/DirectXShaderCompiler/releases

set(SHADERMAKE_FIND_DXC_SPIRV ${CAMEL_NVRHI_VULKAN} CACHE BOOL "" FORCE)
set(SHADERMAKE_FIND_FXC ${CAMEL_NVRHI_DX_11} CACHE BOOL "" FORCE)
set(SHADERMAKE_FIND_DXC ${CAMEL_NVRHI_DX_12} CACHE BOOL "" FORCE)
set(SHADERMAKE_BIN_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

CPMAddPackage(
	NAME
		shadermake
	GITHUB_REPOSITORY
		NVIDIAGameWorks/ShaderMake
	GIT_TAG
		d1b47ff7bc36530c2bc7fa84068ed043ecad5d3d
)