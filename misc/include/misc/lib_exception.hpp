#pragma once

#include <string>
#include <exception>

namespace misc
{
class lib_exception : public std::exception
{
public:
	lib_exception()
		: _exception_message("custom lib: exception thrown")
	{
	}

	explicit lib_exception(std::string_view exception_message)
		: _exception_message(exception_message)
	{
	}

	[[nodiscard]] const char* what() const noexcept override
	{
		return _exception_message.c_str();
	}

private:
	std::string _exception_message = {};
};
}
