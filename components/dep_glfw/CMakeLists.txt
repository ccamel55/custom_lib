# No X11, I don't use old xorg stuff. No install too
CPMAddPackage(
	NAME
		glfw
	GITHUB_REPOSITORY
		glfw/glfw
	GIT_TAG
		3.4
	OPTIONS
		"GLFW_BUILD_X11 OFF"
		"GLFW_INSTALL ${LIB_INSTALL_DEPENDENCIES}"
)

add_dependency(
	glfw
	DEPENDENCIES
		glfw
)