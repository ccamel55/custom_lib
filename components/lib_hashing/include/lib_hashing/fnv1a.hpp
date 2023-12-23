#pragma once

#include <cstdint>
#include <cstring>

namespace lib::common
{
using fnv1a_t = uint32_t;

namespace fnv1a
{
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

constexpr fnv1a_t FNV_PRIME = 0x01000193;
constexpr fnv1a_t FNV_OFFSET_BASIS = 0x811c9dc5;

consteval fnv1a_t get_compile_time(const char* string, fnv1a_t value = FNV_OFFSET_BASIS)
{
	if (string[0] == '\0')
	{
		return value;
	}

	value ^= string[0];
	value *= FNV_PRIME;

	return get_compile_time(string + 1, value);
}

inline fnv1a_t get(const char* string)
{
	fnv1a_t value = FNV_OFFSET_BASIS;

	for (size_t i = 0; i < strlen(string); i++)
	{
		value ^= string[i];
		value *= FNV_PRIME;
	}

	return value;
}
}  // namespace fnv1a
}  // namespace lib::common