#pragma once

#include <cstdint>

// note: these memory functions only work with x86 windows
#if _WIN32 || _WIN64

namespace lib
{
//! Returns the virtual function found at \a index in the \a class_ptr.
template <typename t = void*> inline t get_virtual_function(void* class_ptr, size_t index)
{
	return (*static_cast<t**>(class_ptr))[index];
}

//! Call a virtual function found at \a index in the \a class_ptr.
template <typename t, typename... a> inline t call_virtual_function(void* class_ptr, size_t index, a... args)
{
	using fn_sig = t(__thiscall*)(void*, decltype(args)...);
	return (*static_cast<fn_sig**>(class_ptr))[index](class_ptr, args...);
}

//! Call a function found at \a fn_address using thiscall.
template <typename t, typename... a> inline t call_function(void* ecx, uintptr_t fn_address, a... args)
{
	using fn_sig = t(__thiscall*)(void*, decltype(args)...);
	return reinterpret_cast<fn_sig>(fn_address)(ecx, args...);
}
}  // namespace lib

#endif