#pragma once

#include <concepts>
#include <string_view>
#include <tuple>

// todo: list of todos are the following
//  - allow for custom getter/setters, defaulting to ptr setter and getter if nothing is specified
//  - allow for more complex types (linked to previous todo)

/*
    Example usage:
    struct example_data_struct
    {
        int my_int_1 = 0;
        long my_long_1 = 0l;
        float my_float_1 = 0.f;
        double  my_double_1 = 0.0;

        // a constexpr static function named get_metadata must be defined for each class that wishes to be/
        // serializable
        static constexpr auto get_metadata()
        {
            return {

            serializable(example_data_struct, my_int_1)
            serializable(example_data_struct, my_long_1)
            serializable(example_data_struct, my_float_1)
            serializable(example_data_struct, my_double_1)

           	};
        }
    };
*/

namespace lib
{
template<typename container>
concept serializable = requires {
    { container::get_metadata() };
};

//! The struct that will hold our member metadata. It includes the name and a class ptr to the member it's self.
template<typename container, typename member_type>
struct member_metadata {
    constexpr member_metadata(
            std::string_view member_name,
            member_type container::* member_ptr)
            : member_name(member_name)
            , member_ptr(member_ptr) {
    }

    std::string_view member_name;
    member_type container::* member_ptr;
};

//! Returns the number of "metadata" elements defined in a container.
template<typename container>
constexpr auto get_metadata_size() {
    // get the tuple type from the function "get_metadata" and parse parameters to derive the number of
    // items within that tuple
    using tuple = std::invoke_result_t<decltype(container::get_metadata)>;
    constexpr auto tuple_size = std::tuple_size_v<tuple>;

    return tuple_size;
}

// https://stackoverflow.com/a/67975523
//! Helper function to convert a \a std::integer_sequence into a \a std::integral_constant
template <typename size_type, size_type... size, typename fn>
constexpr void for_sequence(std::integer_sequence<size_type, size...>, fn&& f) {
    (static_cast<void>(f(std::integral_constant<size_type, size>{})), ...);
}

//! Calls the \p cb for each element defined as "metadata' for the type \a container
template<serializable container, typename callback>
constexpr void for_each_metadata(callback&& cb) {
    for_sequence(std::make_index_sequence<get_metadata_size<container>()>{}, [&](auto i) {
        constexpr auto tuple = container::get_metadata();
        constexpr auto property = std::get<i>(tuple);

        cb(property);
    });
}

//! Use this macro to define a member that should be serializable.
#define serializable(class_name, member) \
    lib::member_metadata(#member, &class_name::member),
}