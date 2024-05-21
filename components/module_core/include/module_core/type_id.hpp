#pragma once
#include <type_traits>

namespace lib
{
// typeid implementation without rtti
template<typename T>
struct type_info_t {
    // It doesn't matter what this value is, we will use the memory address as ID rather than the value
    // - for each different type "T" is, we will create a new instance of "id".
    // - this does mean its type identification properties is limited to one library/module only.
    constexpr static int id = 69420;
};

//! Returns a unique identifier for a specific type T
//! \note this will only work in the same library/module.
//! \return unique ID for type T, ID is stored in type sizeof(uintptr_t) (aka word size)
template<typename T>
constexpr inline uintptr_t type_id() {
    return reinterpret_cast<uintptr_t>(&type_info_t<std::decay<T>>::id);
}
}