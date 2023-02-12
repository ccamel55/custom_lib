#pragma once

#include <cstring>
#include <common/types/hash/hash.hpp>

namespace lib::common::fnv1a
{
	// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

	constexpr hash_t FNV_PRIME = 0x01000193;
	constexpr hash_t FNV_OFFSET_BASIS = 0x811c9dc5;

	constexpr hash_t get_compile_time(const char* string, hash_t value = FNV_OFFSET_BASIS)
	{
		if (string[0] == '\0')
		{
			return value;
		}

		value ^= string[0];
		value *= FNV_PRIME;

		return get_compile_time(string + 1, value);
	}

	inline hash_t get(const char* string)
	{
		hash_t value = FNV_OFFSET_BASIS;

		for (size_t i = 0; i < strlen(string); i++)
		{
			value ^= string[i];
			value *= FNV_PRIME;
		}

		return value;
	}
}