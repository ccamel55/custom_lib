#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include <module_memory/type/address.hpp>

namespace lib::hook
{
//! Inline hooking manager.
//! All functions should be thread safe.
class InlineHook {
    struct hook_object_t {

    };

public:
    //! Create an inline hook.
    //! \param original_function address of original function
    //! \param hooked_function address of hooked function
    //! \param trampoline_function mutable reference to store address of trampoline function
    //! \return true if hook was created successfully, false otherwise.
    bool emplace(
        lib::memory::address original_function,
        lib::memory::address hooked_function,
        lib::memory::address& trampoline_function
    );

    //! Remove an inline hook.
    //! \param original_function address of original function
    //! \return true if hook was removed successfully, false otherwise.
    bool pop(
        lib::memory::address original_function
    );

    //! Patch hooked functions with respective JMPs
    //! \return true if all functions where patched, false otherwise.
    bool apply();

    //! Restore patched functions to original behavior.
    //! \return true if all functions where restored, false otherwise.
    bool restore();

private:
    std::mutex _lock                                        = {};
    std::unordered_map<uintptr_t, hook_object_t> _object    = {};

};
}