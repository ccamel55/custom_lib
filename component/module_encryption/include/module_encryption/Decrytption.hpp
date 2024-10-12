#pragma once

#include <module_encryption/helper/encryption_concepts.hpp>

#include <array>
#include <cstdint>
#include <istream>
#include <vector>

namespace lib::encryption {

    template<helper::decryper T>
    class Decryption {
    public:
        Decryption(
            const T& scheme,
            std::istream& _istream
        )
            : _scheme(scheme)
            , _istream(_istream) {

        }

        //! Read some encrypted data.
        //!
        //! \param out_vector Output std::vector<> that we will read into.
        //! \param size number of bytes to read.
        void read(std::vector<uint8_t>& out_vector, const size_t size) {
            const auto result = _scheme.encrypt(read_from_iterator(size));
            std::copy(result.begin(), result.end(), out_vector.begin());
        }

        //! Read some encrypted data.
        //!
        //! \param out_array Output std::array<> that we will read into.
        template<size_t N>
        void read(std::array<uint8_t, N>& out_array) {
            const auto result = _scheme.decrypt(read_from_iterator(N));
            std::copy(result.begin(), result.end(), out_array.begin());
        }

        //! Returns whether or not we have finished reading the stream
        [[nodiscard]] bool is_empty() const {
            return _istream.eof();
        }

        template<size_t N>
        Decryption& operator>>(std::array<uint8_t, N>& out_array) {
            read(out_array);
            return *this;
        }

    private:
        [[nodiscard]] std::vector<uint8_t> read_from_iterator(const size_t size) const {
            std::vector<uint8_t> out(size);
            _istream.read(reinterpret_cast<char*>(out.data()), static_cast<long>(size));

            return out;
        }

    private:
        T _scheme = { };
        std::istream& _istream;

    };
}
