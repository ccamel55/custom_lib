#pragma once

#include <cstdint>
#include <type_traits>

namespace lib::memory
{
namespace detail
{
//! Struct containing helper functions for dealing with pointers.
template<typename Ptr = uintptr_t> requires std::is_unsigned_v<Ptr>
struct address_object {
    address_object() = default;

    //! Create address object from an integer.
    //! \param ptr address the object represents
    explicit address_object(Ptr ptr) : raw(ptr) {};

    //! Create address object from a pointer.
    //! \param ptr address the object represents
    explicit address_object(Ptr* ptr) : raw(reinterpret_cast<Ptr>(ptr)) {};

    //! Create address object from a pointer.
    //! \param ptr address the object represents
    explicit address_object(const Ptr* ptr) : raw(reinterpret_cast<Ptr>(ptr)) {};

    //! Create address object from a pointer.
    //! \param ptr address the object represents
    explicit address_object(void* ptr) : raw(reinterpret_cast<Ptr>(ptr)) {};

    //! Create address object from a pointer.
    //! \param ptr address the object represents
    explicit address_object(const void* ptr) : raw(reinterpret_cast<Ptr>(ptr)) {};

    //! Offset the current address object
    //! \param size how much to offset. number of bytes = size * sizeof(T)
    //! \return offset address
    template<typename T = uint8_t>
    [[nodiscard]] address_object offset(ptrdiff_t size) const {
        return address_object(raw + size * sizeof(T));
    }

    //! Deference the address and use that as address object
    //! \return dereferenced address
    [[nodiscard]] address_object dereference() const {
        return address_object(*reinterpret_cast<Ptr*>(raw));
    }

    //! Dereference and get the value address represents.
    //! \return reference to the value
    template<typename T>
    [[nodiscard]] T& get() const {
        return *reinterpret_cast<T*>(raw);
    }

    //! Get the current address as type of pointer
    //! \return pointer of type T
    template<typename T>
    [[nodiscard]] T* ptr() const {
        return reinterpret_cast<T*>(raw);
    }

    //! Raw dog cast the address.
    //! \return address as T
    template<typename T>
    [[nodiscard]] T cast() const {
        return reinterpret_cast<T>(raw);
    }

    bool operator==(const address_object& in) {
        return this->raw == in.raw;
    }

    Ptr raw = 0;

};

}

using address = detail::address_object<uintptr_t>;
using address_32 = detail::address_object<uint32_t>;
using address_64 = detail::address_object<uint64_t>;

}