#pragma once

#include <module_hashing/fnv1a_32.hpp>

#include <cassert>
#include <cstdint>

namespace lib::encryption::helper {
    //! Generate a random 32 bit seed based on __TIME__ during compile time.
    //!
    //! \return 32 bit seed
    constexpr uint32_t seed_32() {
        if (!std::is_constant_evaluated()) {
            assert(false && "seed_32() was constantly valued, you done fucked up!");
        }

        return hashing::fnv1a_32(__TIME__);
    }
}
