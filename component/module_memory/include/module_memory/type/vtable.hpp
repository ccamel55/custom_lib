#pragma once

#include <optional>
#include <type_traits>

#include <module_memory/type/address.hpp>
#include <module_memory/type/memory_section.hpp>

namespace lib::memory
{
struct vtable {

    //! Create an object that represents the currently set vtable. If the Vtable ptr changes
    //! this will not update.
    constexpr vtable(address vtable_ptr_address)
        : table({}) {

        // Find size by walking the table
        size_t size;
        const auto vtable = vtable_ptr_address.dereference().ptr<uintptr_t>();

        for (size = 0; vtable[size]; size++) {
            // Yum, thanks microsoft!
            if ((((unsigned long)(vtable[size])) >> 16) == 0) {
                break;
            }
        }

        std::span<std::byte> table_span(reinterpret_cast<std::byte*>(vtable), size * sizeof(uintptr_t));
        table = memory_section(table_span);
    }

    //! Get the address of a function at index in the vtable
    //! \param index vtable index
    //! \return address to function found at index
    [[nodiscard]] constexpr std::optional<address> get(size_t index) const {
        if (index >= size()) {
            return std::nullopt;
        }

        return table.base()
            .offset<uintptr_t>(static_cast<ptrdiff_t>(index))
            .dereference();
    }

    //! Get the size of the vtable
    //! \return size of vtable
    [[nodiscard]] constexpr size_t size() const {
        return table.size<uintptr_t>();
    }

    constexpr bool operator==(const vtable& in) const {
        return this->table == in.table;
    }

    memory_section table;
};
}