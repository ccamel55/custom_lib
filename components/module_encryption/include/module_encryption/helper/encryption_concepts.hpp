#pragma once

#include <concepts>
#include <cstdint>
#include <vector>

namespace lib::encryption::helper {

    //! Concept that every encryption implementation must confine too.
    template<typename T>
    concept encrypter = requires(T t) {
        { t.encrypt(std::vector<uint8_t>()) } -> std::convertible_to<std::vector<uint8_t>>;
    };

    //! Concept that every decryption implementation must confine too.
    template<typename T>
    concept decryper = requires(T t) {
        { t.decrypt(std::vector<uint8_t>()) } -> std::convertible_to<std::vector<uint8_t>>;
    };
}