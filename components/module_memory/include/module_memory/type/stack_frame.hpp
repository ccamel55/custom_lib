#pragma once

#include <module_memory/type/address.hpp>

namespace lib::memory
{
//! Struct that represents the current method's stack frame
struct stack_frame{

    constexpr stack_frame(void* frame_pointer, void* return_address)
        : stack_frame(reinterpret_cast<uintptr_t>(frame_pointer), reinterpret_cast<uintptr_t>(return_address)) {
    }

    constexpr stack_frame(uintptr_t frame_pointer, uintptr_t return_address)
        : frame_pointer(frame_pointer)
        , return_address(return_address) {
    }

    //! Get a variable relative to the frame pointer
    //! \param index index from the frame pointer
    //! \return address to the variable
    [[nodiscard]] constexpr address get(int32_t index) const {
        return frame_pointer.offset<uintptr_t>(index);
    }

    constexpr bool operator==(const stack_frame& in) {
        return this->frame_pointer == in.frame_pointer
            && this->return_address == in.return_address;
    }

    address frame_pointer;
    address return_address;

};

//! Get the current stack frame.
#define STACK_FRAME stack_frame(__builtin_frame_address(0), __builtin_return_address(0));

}