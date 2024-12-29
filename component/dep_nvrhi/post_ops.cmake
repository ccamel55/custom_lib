if (CAMEL_NVRHI_VULKAN)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
			nvrhi_vk
			Vulkan-Headers
	)
endif()

if (CAMEL_NVRHI_DX_11)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		nvrhi_d3d11
		d3d11
		dxgi
	)
endif()

if (CAMEL_NVRHI_DX_12)
	target_link_libraries(
		${PROJECT_NAME} PUBLIC
		nvrhi_d3d12
		d3d12
		dxgi
	)
endif()

# STUPID NOTE: NVRHI must be linked AFTER nvrhi_vulkan etc.
target_link_libraries(
	${PROJECT_NAME} PUBLIC
	nvrhi
	ShaderMakeBlob
)