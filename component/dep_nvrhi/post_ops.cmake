target_link_libraries(
	${PROJECT_NAME} PUBLIC
		nvrhi
		nvrhi_vk
)

#
# Add Vulkan include and libraries
#
target_include_directories(
	${PROJECT_NAME} PUBLIC
		${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
		${Vulkan_LIBRARIES}
)

if (CURRENT_PLATFORM STREQUAL "darwin")
	set(_darwin_compile_definitions "-DVK_ENABLE_BETA_EXTENSIONS")

	# SRS - Enable Beta extensions for VULKAN_SDK portability subset features on OSX
	target_compile_definitions(${PROJECT_NAME} ${_darwin_compile_definitions})
	target_compile_definitions(nvrhi_vk} ${_darwin_compile_definitions})
endif()

#
# We didn't make this library so i don't care about the warnings.
#
set(NVRHI_COMPILE_FLAGS "-Wno-unused-private-field" "-Wno-unused-command-line-argument" "-Wno-deprecated-copy" "-Wno-strict-aliasing")

target_compile_options(nvrhi PRIVATE
	${NVRHI_COMPILE_FLAGS}
)

target_compile_options(nvrhi_vk PRIVATE
	${NVRHI_COMPILE_FLAGS}
)
