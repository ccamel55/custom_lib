#pragma once

#include <concepts>
#include <optional>
#include <type_traits>
#include <unordered_map>

#include <module_memory/type/address.hpp>

namespace lib::hook
{
using hook_id               = size_t;
static hook_id ID_COUNTER   = 0;

// This concept will be used to ensure all platform APIs match in compile time!
template<typename T>
concept DetourApi = requires(T t, lib::memory::address address, hook_id id)
{
    //! Hook a function
    //! \param function The function to hook
    //! \param replacement The replacement function
    //! \param original The address of the original function
    //! \return hook id
    { t.intercept(address, address, address) } -> std::same_as<std::optional<hook_id>>;

    //! Restore hook to a function
    //! \param id The hook id of the function to restore
    //! \return boolean whether or not function is restored
    { t.restore(id) } -> std::same_as<bool>;
};
//
//#ifdef _WIN32
//#include <dep_minhook/minhook.hpp>
//
////! Windows Minhook implementation of DetourApi
//class Detour {
//public:
//    Detour() {
//        MH_Initialize();
//    }
//
//    ~Detour() {
//        MH_Uninitialize();
//    }
//
//    [[nodiscard]] std::optional<hook_id> intercept(
//        const lib::memory::address& function,
//        const lib::memory::address& replacement,
//        lib::memory::address& original
//    ) {
//        LPVOID original_fn = nullptr;
//        const auto create_hook = MH_CreateHook(
//            function.cast<LPVOID>(),
//            replacement.cast<LPVOID>(),
//            &original_fn
//        );
//
//        if (create_hook != MH_OK) {
//            return std::nullopt;
//        }
//
//        const auto enable_hook = MH_EnableHook(function.cast<LPVOID>());
//
//        if (enable_hook != MH_OK) {
//            return std::nullopt;
//        }
//
//        original.raw = reinterpret_cast<decltype(original.raw)>(original_fn);
//
//        const auto id = ID_COUNTER++;
//        _hook_map.insert({id, function});
//
//        return id;
//    }
//
//    [[nodiscard]] bool restore(hook_id id) {
//        if (!_hook_map.contains(id)) {
//            return false;
//        }
//
//        const auto hook_function = _hook_map.at(id);
//        const auto disable_hook = MH_DisableHook(hook_function.cast<LPVOID>());
//
//        if (disable_hook != MH_OK) {
//            return false;
//        }
//
//        _hook_map.erase(id);
//        return true;
//    }
//
//private:
//    std::unordered_map<hook_id, lib::memory::address> _hook_map  = {};
//
//};

//#else
//#error "module_memory currently only supports windows"
//#endif
}