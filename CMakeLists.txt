cmake_minimum_required(VERSION 3.26)

project(
	camel_lib
	LANGUAGES
		CXX
	DESCRIPTION
		"ccamel55's multi purpose library!"
)

# Add cmake module paths
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Run library cmake code
include(main)
