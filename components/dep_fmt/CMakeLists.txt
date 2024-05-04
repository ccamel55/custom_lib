CPMAddPackage(
	NAME fmt
	GITHUB_REPOSITORY fmtlib/fmt
	GIT_TAG 10.2.1
)

add_dependency(
	fmt
	DEPENDENCIES
		fmt::fmt
)