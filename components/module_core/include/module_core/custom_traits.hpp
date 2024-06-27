#pragma once

#include <functional>
#include <type_traits>

// More template/type traits that I use/think is useful

namespace lib {
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

// ===================================================================================

//! Whether or not a type T exists in the parameter pack Types...
template<typename T, typename... Types>
struct is_type_present {
    // (std::is_same_v<T, Types> || ...) expands to
    // (std::is_same_v<T, Types[0]> || std::is_same_v<T, Types[1]> ...)
    constexpr static bool value{ (std::is_same_v<T, Types> || ...) };
};

//! Returns a constexpr bool representing if type T exists in the parameter pack Types...
//! \return constexpr bool
template<typename T, typename... Types>
constexpr inline bool is_type_present_v = is_type_present<T, Types...>::value;

// ===================================================================================

//! Iterate over an integer sequence by passing an std::integral_constant of type T for each element in the
//! integer sequence to the callback function.
//! \arg callback The callback which accepts an integral sequence of type T
template<
    typename Fn,
    typename T, T... element
>
constexpr inline void iterate_integer_sequence(
    [[maybe_unused]] std::integer_sequence<T, element...>,
    Fn&& callback
) {
    // We use pack expression to call the callback for each value of the integer sequence
    (callback(std::integral_constant<T, element>{ }), ...);
}

// ===================================================================================


}
