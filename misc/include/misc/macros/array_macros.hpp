#pragma once

namespace lib
{
//! Create a filled vector with \a fill. of size \a s.
template <typename t, size_t s> inline std::vector<t> CREATE_FILLED_VECTOR(const t& fill)
{

	std::vector<t> ret(s);
	std::fill(ret.begin(), ret.begin() + s, fill);

	return ret;
}
}  // namespace lib