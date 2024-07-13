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
		"NVRHI_INSTALL ${LIB_INSTALL_DEPENDENCIES}"
		"NVRHI_WITH_VULKAN ON"
		"NVRHI_WITH_DX11 OFF"
		"NVRHI_WITH_DX12 OFF"
)

add_dependency(
	nvrhi
	DEPENDENCIES
		nvrhi
		nvrhi_vk
)

target_include_directories(dep_nvrhi PUBLIC
	${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(dep_nvrhi PUBLIC
	${Vulkan_LIBRARIES}
)

if (CURRENT_PLATFORM STREQUAL "darwin")
	set(_darwin_compile_definitions "-DVK_ENABLE_BETA_EXTENSIONS")

	# SRS - Enable Beta extensions for VULKAN_SDK portability subset features on OSX
	target_compile_definitions(${dep_nvrhi} ${_darwin_compile_definitions})
	target_compile_definitions(${nvrhi_vk} ${_darwin_compile_definitions})
endif()

# We didn't make this library so i don't care about the warnings.
set(NVRHI_COMPILE_FLAGS "-Wno-unused-private-field -Wno-unused-command-line-argument -Wno-deprecated-copy -Wno-strict-aliasing")

set_target_properties(nvrhi PROPERTIES
	COMPILE_FLAGS ${NVRHI_COMPILE_FLAGS}
)

set_target_properties(nvrhi_vk PROPERTIES
	COMPILE_FLAGS ${NVRHI_COMPILE_FLAGS}
)


