#include <catch2/catch_test_macros.hpp>

#include <module_encryption/Decrytption.hpp>
#include <module_encryption/Encryption.hpp>

namespace {
    class FakeEncrypter {
    public:
        [[nodiscard]] std::vector<uint8_t> encrypt(const std::vector<uint8_t>& input) {
            return input;
        }

    };

    class FakeDecrypter {
    public:
        [[nodiscard]] std::vector<uint8_t> decrypt(const std::vector<uint8_t>& input){
            return input;
        }

    };
}

TEST_CASE("Encryption - test", "[encryption]") {
    std::stringstream some_ss;
    some_ss << std::noskipws << "My name jeff " << "hahaha im so funny " << "i love cats" << "\n";

    // Setup shit with the encrypter before
    FakeEncrypter encrypter = {};

    // Use the bad boy to do our work!!
    lib::encryption::Encryption encryption(encrypter, some_ss);

    std::array<uint8_t, 12> encrypted_data = {};
    encryption >> encrypted_data;

    std::string my_string(encrypted_data.begin(), encrypted_data.end());
    REQUIRE(my_string == "My name jeff");
}

TEST_CASE("Decryption - test", "[encryption]") {
    std::stringstream some_ss;
    some_ss << std::noskipws << "My name jeff " << "hahaha im so funny " << "i love cats" << "\n";

    // Setup shit with the encrypter before
    FakeDecrypter decrypter = {};

    // Use the bad boy to do our work!!
    lib::encryption::Decryption decryption(decrypter, some_ss);

    std::array<uint8_t, 12> decrypted_data = {};
    decryption >> decrypted_data;

    std::string my_string(decrypted_data.begin(), decrypted_data.end());
    REQUIRE(my_string == "My name jeff");
}

