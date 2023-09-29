#pragma once

namespace lib
{
//! Classes that inherit singleton can only be accessed using the singleton \c get function.
template <class t> class singleton
{
public:
	static t& get()
	{
		// instance will be created on first use and destroyed when program is killed.
		static t singleton_instance;
		return singleton_instance;
	}

	// Prevent copying the singleton.
	singleton(const singleton&) = delete;
	void operator=(const singleton&) = delete;

private:
	// Ensure constructor and destructor is only callable from the singleton.
	singleton() = default;
	~singleton() = default;
};
}  // namespace lib