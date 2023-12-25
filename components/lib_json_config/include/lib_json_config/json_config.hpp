#pragma once

#include <string>
#include <vector>

namespace lib::json_config
{
enum class value_type: uint8_t
{
    boolean,
    integer,
    floating_point,
    string,
    boolean_array,
    integer_array,
    floating_point_array,
    string_array,
};

struct config_value_t
{
    value_type type = value_type::boolean;
    void* value_ptr = nullptr;
};

class config_manager;

class json_config
{
    friend config_manager;

public:
    template<typename t>
    constexpr void add_value(const std::string& name, value_type type, t& value)
    {
        _values.insert({name, config_value_t{
            .type = type,
            .value_ptr = static_cast<void*>(&value)
        }});
    }

private:
    std::unordered_map<std::string, config_value_t> _values = {};

};
}
