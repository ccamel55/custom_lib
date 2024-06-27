#pragma once
#include <cstdint>

namespace lib {
using bitflag_t = uint32_t;

class bitflag {
public:
    constexpr bitflag()  = default;
    ~bitflag() = default;

    // allow implicit conversion, we want this!
    constexpr bitflag(bitflag_t flag)
        : _flag(flag) {
    }

    //! Add the bit specified by \param flag
    constexpr void add(bitflag_t flag) {
        _flag |= flag;
    }

    //! Unset the bit specified by \param flag
    constexpr void remove(bitflag_t flag) {
        _flag &= ~flag;
    }

    //! Set the bit specified by \param flag
    [[nodiscard]] constexpr bool has(bitflag_t flag) const {
        return _flag & flag;
    }

    //! Flip the bit specified by \param flag
    constexpr void toggle(bitflag_t flag) {
        _flag ^= flag;
    }

    constexpr bool operator==(const bitflag& in) const {
        return this->_flag == in._flag;
    }

private:
    bitflag_t _flag = 0;

};
}
