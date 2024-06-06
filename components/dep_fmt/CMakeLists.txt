# Don't install FMT library
set(FMT_INSTALL ${LIB_INSTALL_DEPENDENCIES})

add_dependency(
	fmt
	GITHUB_REPOSITORY
		fmtlib/fmt
	GIT_TAG
		10.2.1
	DEPENDENCIES
		fmt::fmt
)