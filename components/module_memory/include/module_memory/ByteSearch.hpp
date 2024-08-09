#pragma once

#include <optional>
#include <string>
#include <vector>

#include <module_memory/type/memory_section.hpp>
#include <module_memory/ByteSearchHelper.hpp>

namespace lib::memory
{
//! Helper to find the address of a byte pattern
//! All functions are thread safe.
class ByteSearch {
public:
    explicit ByteSearch(memory_section memory);

    //! Find address of first match for sequence of bytes
    //! \param byte_pattern byte pattern sequence
    //! \return optional containing address of pattern object.
    [[nodiscard]] std::optional<address> find(const std::vector<int>& byte_pattern) const;

private:
    memory_section _memory;

};
}