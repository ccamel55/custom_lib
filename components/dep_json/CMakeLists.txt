CPMAddPackage(
	NAME
		json
	GITHUB_REPOSITORY
		nlohmann/json
	GIT_TAG
		v3.11.3
)

add_dependency(
	json
	DEPENDENCIES
		nlohmann_json::nlohmann_json
)