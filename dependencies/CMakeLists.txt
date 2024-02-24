add_subdirectory(cxxopts)
add_subdirectory(fmt)
add_subdirectory(glm)
add_subdirectory(json)

# GLM preprocessors
target_compile_definitions(glm PUBLIC
	GLM_FORCE_CXX17
	GLM_FORCE_INLINE
	GLM_FORCE_INTRINSICS
	GLM_ENABLE_EXPERIMENTAL
)