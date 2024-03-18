#pragma once

#include <core_sdk/templates/no_copy.hpp>

namespace lib
{
//! Classes that inherit singleton can only be accessed using the singleton \c get function.
template <class t>
class singleton : public no_copy
{
public:
	static t& get()
	{
		// instance will be created on first use and destroyed when program is killed.
		static t singleton_instance = {};
		return singleton_instance;
	}

protected:
	// Ensure constructor and destructor is only callable from the singleton.
	singleton() = default;
	~singleton() = default;
};
}  // namespace lib