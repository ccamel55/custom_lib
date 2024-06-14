#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace lib::hashing
{
    //! Alias for representing fnv1a_32 hashes
    using fnv1a_32_t = uint32_t;

    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    namespace detail
    {
        constexpr fnv1a_32_t FNV_PRIME = 0x01000193;
        constexpr fnv1a_32_t FNV_OFFSET_BASIS = 0x811c9dc5;

        // Compile time fnv1a 32 bit hashing function. This uses recursion so I want to avoid calling
        // it unless we are doing compile time evaluation.
        [[nodiscard]] constexpr fnv1a_32_t fnv1a_32_ct(
            const char* string,
            fnv1a_32_t hash = detail::FNV_OFFSET_BASIS
        ) {
            // Reach null terminator, therefore we have generated the whole hash.
            if (string[0] == '\0') {
                return hash;
            }

            hash ^= string[0];
            hash *= detail::FNV_PRIME;

            // WOAH recursion? it's ok, we do this compile time!
            return fnv1a_32_ct(string + 1, hash);
        }
    }

    //! fnv1a 32 bit hashing function
    //! \param string The string that will be hashed.
    [[nodiscard]] constexpr fnv1a_32_t fnv1a_32(const char* string) {
        // function evaluated at compile time? good! we will call recursive function and generate
        // hash during compile, otherwise use looped version for runtime.
        if (std::is_constant_evaluated()) {
            return detail::fnv1a_32_ct(string);
        }

        fnv1a_32_t hash = detail::FNV_OFFSET_BASIS;

        for (size_t i = 0; i < strlen(string); i++) {
            hash ^= string[i];
            hash *= detail::FNV_PRIME;
        }

        return hash;
    }
}