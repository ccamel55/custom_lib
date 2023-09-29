#pragma once

#include <cstdint>

namespace lib::common
{
using bitflag_t = uint32_t;

class bitflag
{
public:
	bitflag() = default;

	explicit bitflag(bitflag_t flags) : _flags(flags)
	{
	}

	//! Set the bit specified by \param flag
	inline void set_flag(bitflag_t flag)
	{
		_flags |= flag;
	}

	//! Unset the bit specified by \param flag
	inline void remove_flag(bitflag_t flag)
	{
		_flags &= ~flag;
	}

	//! Set the bit specified by \param flag
	[[nodiscard]] inline bool has_flag(bitflag_t flag) const
	{
		return _flags & flag;
	}

	//! Flip the bit specified by \param flag
	inline void toggle_flag(bitflag_t flag)
	{
		_flags ^= flag;
	}

private:
	bitflag_t _flags = 0;
};
}  // namespace lib::common