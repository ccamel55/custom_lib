#pragma once

#include <string>
#include <string_view>

#include <vector>
#include <ranges>

namespace lib::memory
{

//! Convert an IDA style pattern string into an array of bytes
//! \param ida_pattern IDA style pattern
//! \return byte pattern
[[nodiscard]] constexpr std::vector<int> bytes_from_ida_pattern(std::string_view ida_pattern) {
    return ida_pattern
        | std::views::split(' ')
        | std::views::filter([](const auto& i) {
            const auto i_str = std::string_view(i);
            return !(i_str.empty() || std::string_view(i).contains(' '));
        })
        | std::views::transform([](const auto& i) {
            const auto i_str = std::string_view(i);
            if (i_str.contains('?')) {
                return -1;
            }
            return static_cast<int>(std::strtoul(i_str.data(), nullptr, 16));
        })
        | std::ranges::to<std::vector>();
}
}