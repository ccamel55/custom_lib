#pragma once
#include <cstdint>

namespace lib {
using bitflag_t = uint32_t;

//! 32-bit flag type
class bitflag {
public:
    constexpr bitflag()  = default;

    constexpr bitflag(bitflag_t flag)
        : _flag(flag) {
    }

    //! Add a bit
    //! \param flag bit to add
    constexpr void add(bitflag_t flag) {
        _flag |= flag;
    }

    //! Add a bit
    //! \param flag bit to add
    constexpr void add(bitflag flag) {
        _flag |= flag._flag;
    }

    //! Remove a bit
    //! \param flag bit to remove
    constexpr void remove(bitflag_t flag) {
        _flag &= ~flag;
    }

    //! Remove a bit
    //! \param flag bit to remove
    constexpr void remove(bitflag flag) {
        _flag &= ~flag._flag;
    }

    //! Check if flag has a bit
    //! \param flag bit to check
    //! \return whether or not flag is preset
    [[nodiscard]] constexpr bool has(bitflag_t flag) const {
        return _flag & flag;
    }

    //! Check if flag has a bit
    //! \param flag bit to check
    //! \return whether or not flag is preset
    [[nodiscard]] constexpr bool has(bitflag flag) const {
        return _flag & flag._flag;
    }

    //! Toggle the state of a bit
    //! \param flag bit to toggle
    constexpr void toggle(bitflag_t flag) {
        _flag ^= flag;
    }

    //! Toggle the state of a bit
    //! \param flag bit to toggle
    constexpr void toggle(bitflag flag) {
        _flag ^= flag._flag;
    }

    constexpr bool operator==(const bitflag_t & in) const {
        return this->_flag == in;
    }

    constexpr bool operator==(const bitflag& in) const {
        return this->_flag == in._flag;
    }

private:
    bitflag_t _flag = 0;

};
}
