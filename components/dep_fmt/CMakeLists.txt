CPMAddPackage(
	NAME
		fmt
	GITHUB_REPOSITORY
		fmtlib/fmt
	GIT_TAG
		10.2.1
	OPTIONS
		"FMT_INSTALL ${LIB_INSTALL_DEPENDENCIES}"
)

add_dependency(
	fmt
	DEPENDENCIES
		fmt::fmt
)