#pragma once

#include <array>
#include <cstdint>

namespace lib::common
{
// md5 is 128 bit :O
using md5_t = std::array<uint8_t, 16>;

namespace md5
{

}
}  // namespace lib::common