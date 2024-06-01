# No X11, I don't use old xorg stuff.
set(GLFW_BUILD_X11 OFF)

add_dependency(
	glfw
	GITHUB_REPOSITORY
		glfw/glfw
	GIT_TAG
		3.4
	DEPENDENCIES
		glfw
)