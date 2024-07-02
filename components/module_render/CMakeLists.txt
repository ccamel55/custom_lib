add_module(
	render
	DEPENDENCIES
		dep_nvrhi
		module_core
)

include(cmake/compile_hlsl.cmake)

