#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <cmath>

#include <module_memory/type/address.hpp>

namespace lib::memory
{
//! Represents a section in memory
struct memory_section
{
    constexpr memory_section(const std::span<std::byte>& section)
        : section(section) {
    }

    //! Get the base address of the memory section
    //! \return base address of memory section
    [[nodiscard]] constexpr address base() const {
        return {section.data()};
    }

    //! Get the size of memory section for type T. If memory section is not divisible by sizeof(t)
    //! you will receive the floor result.
    //! \return size of memory section in terms of T (total bytes = size() * sizeof(T))
    template<typename T = std::byte>
    [[nodiscard]] constexpr size_t size() const {
        return static_cast<size_t>(std::floor(static_cast<float>(section.size_bytes()) / static_cast<float>(sizeof(T))));
    }

    //! Checks if memory section contains a specific address
    //! \param in address to check
    //! \return whether or not in exists in memory section
    [[nodiscard]] constexpr bool contains(address in) const {
       return in >= base() && in <= base().offset(static_cast<ptrdiff_t>(size()));
    }

    constexpr bool operator==(const memory_section& in) const {
        return section.data() == in.section.data()
            && section.size_bytes() == in.section.size_bytes();
    }

    std::span<std::byte> section;
};
}