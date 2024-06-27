#pragma once

#include <module_memory/type/address.hpp>

namespace lib::memory
{
//! Struct that represents the current method's stack frame
struct stack_frame{

    stack_frame(void* frame_pointer, void* return_address)
        : stack_frame(reinterpret_cast<uintptr_t>(frame_pointer), reinterpret_cast<uintptr_t>(return_address)) {
    }

    stack_frame(uintptr_t frame_pointer, uintptr_t return_address)
        : frame_pointer(frame_pointer)
        , return_address(return_address) {
    }

    //! Get the address to a variable relative to the frame pointer
    //! \param pointer_offset offset (number of bytes = pointer_offset * sizeof(uintptr_t))
    //! \return pointer of type T to the variable
    template<typename T>
    [[nodiscard]] T* get(ptrdiff_t pointer_offset) const {
        constexpr ptrdiff_t pointer_size = sizeof(uintptr_t);
        return frame_pointer.offset(pointer_offset * pointer_size).ptr<T>();
    }

    //! Get a variable relative to the frame pointer
    //! \param pointer_offset offset (number of bytes = pointer_offset * sizeof(uintptr_t))
    //! \return reference to the variable
    template<typename T>
    [[nodiscard]] T& get_variable(ptrdiff_t pointer_offset) const {
        return *get<T>(pointer_offset);
    }

    address frame_pointer;
    address return_address;

};

//! Get the current stack frame.
#define STACK_FRAME stack_frame(__builtin_frame_address(0), __builtin_return_address(0));

}