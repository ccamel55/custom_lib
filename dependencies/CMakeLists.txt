add_subdirectory(Catch2)
add_subdirectory(cxxopts)
add_subdirectory(fmt)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(json)
add_subdirectory(VulkanMemoryAllocator)

# GLM preprocessors
target_compile_definitions(glm PUBLIC
	GLM_FORCE_CXX17
	GLM_FORCE_INLINE
	GLM_FORCE_INTRINSICS
	GLM_ENABLE_EXPERIMENTAL
)