CPMAddPackage(
	NAME
		glm
	GITHUB_REPOSITORY
		g-truc/glm
	GIT_TAG
		1.0.1
)

target_compile_definitions(
	glm PUBLIC
		GLM_FORCE_CXX17
		GLM_FORCE_INLINE
		GLM_FORCE_INTRINSICS
		GLM_ENABLE_EXPERIMENTAL
)