#pragma once

#include <optional>
#include <type_traits>

#include <module_memory/type/address.hpp>

namespace lib::memory
{
struct vtable {

    explicit vtable(address vtable_address)
        : table(vtable_address)
        , size(0) {

        // Find size by walking the table
        auto vtable = table.dereference().ptr<uintptr_t>();
        for (size = 0; vtable[size]; size++) {
            // Yum, thanks microsoft!
            if ((((unsigned long)(vtable[size])) >> 16) == 0) {
                break;
            }
        }
    }

    //! Get the address of a function at index in the vtable
    //! \param index vtable index
    //! \return address to function found at index
    [[nodiscard]] std::optional<address> get(size_t index) const {
        if (index >= size) {
            return std::nullopt;
        }

        // We need to dereference each time in case the vtable ptr gets replaced
        // with something else (aka hooked)
        return table.dereference()
            .offset<uintptr_t>(static_cast<ptrdiff_t>(index))
            .dereference();
    }

    bool operator==(const vtable& in) {
        return this->table == in.table;
    }

    address table;
    size_t size;
};
}