#
# We didn't make this library so i don't care about the warnings.
#
set(NVRHI_COMPILE_FLAGS "-Wno-unused-private-field" "-Wno-unused-command-line-argument" "-Wno-deprecated-copy" "-Wno-strict-aliasing")

target_compile_options(
	nvrhi PRIVATE
		${NVRHI_COMPILE_FLAGS}
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
		nvrhi
)

#
# Add include and libraries depending on graphics API
#
if (CAMEL_NVRHI_VULKAN)
	target_include_directories(
		${PROJECT_NAME} PUBLIC
			${Vulkan_INCLUDE_DIRS}
	)

	target_link_libraries(
		${PROJECT_NAME} PUBLIC
			nvrhi_vk
			${Vulkan_LIBRARIES}
	)

	target_compile_options(
		nvrhi_vk PRIVATE
			${NVRHI_COMPILE_FLAGS}
	)
endif()

if (CAMEL_NVRHI_DX_11)
#	target_include_directories(
#		${PROJECT_NAME} PUBLIC
#	)
#
#	target_link_libraries(
#		${PROJECT_NAME} PUBLIC
#			nvrhi_d3d12
#	)
#
#	target_compile_options(
#		nvrhi_d3d12 PRIVATE
#			${NVRHI_COMPILE_FLAGS}
#	)
endif()

if (CAMEL_NVRHI_DX_12)
#	target_include_directories(
#		${PROJECT_NAME} PUBLIC
#	)
#
#	target_link_libraries(
#		${PROJECT_NAME} PUBLIC
#			nvrhi_d3d12
#	)
#
#	target_compile_options(
#		nvrhi_d3d12 PRIVATE
#			${NVRHI_COMPILE_FLAGS}
#	)
endif()


