# Todo: Add support for logging from multiple threads, log to file, different log level prints.

add_module(
	logger
	NO_INSTALL
	DEPENDENCIES
		dep_fmt
		module_core
)