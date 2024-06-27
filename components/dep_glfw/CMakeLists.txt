# No X11, I don't use old xorg stuff. No install too
set(GLFW_BUILD_X11 OFF)
set(GLFW_INSTALL ${LIB_INSTALL_DEPENDENCIES})

add_dependency(
	glfw
	GITHUB_REPOSITORY
		glfw/glfw
	GIT_TAG
		3.4
	DEPENDENCIES
		glfw
)