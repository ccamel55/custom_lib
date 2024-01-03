#pragma once

#include <cstdint>

namespace lib
{
using bitflag_t = uint32_t;

class bitflag
{
public:
    bitflag() = default;
    ~bitflag() = default;

    // allow implicit conversion, we want this!
    inline bitflag(bitflag_t flag)
        : _flag(flag)
    {
    }

    //! Add the bit specified by \param flag
    inline void add(bitflag_t flag)
    {
        _flag |= flag;
    }

    //! Unset the bit specified by \param flag
    inline void remove(bitflag_t flag)
    {
        _flag &= ~flag;
    }

    //! Set the bit specified by \param flag
    [[nodiscard]] inline bool has(bitflag_t flag) const
    {
        return _flag & flag;
    }

    //! Flip the bit specified by \param flag
    inline void toggle(bitflag_t flag)
    {
        _flag ^= flag;
    }

private:
    bitflag_t _flag = 0;

};
}