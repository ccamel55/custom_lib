#pragma once

namespace lib
{
//! Returns the virtual function found at \a index in the \a class_ptr.
template <typename t = void*> t get_virtual_function(void* class_ptr, size_t index)
{
	return (*static_cast<t**>(class_ptr))[index];
}

//! Call a virtual function found at \a index in the \a class_ptr.
template <typename t, typename... a> t call_virtual_function(void* class_ptr, size_t index, a... args)
{
	using fn_sig = t(__thiscall*)(void*, decltype(args)...);
	return (*static_cast<fn_sig**>(class_ptr))[index](class_ptr, args...);
}
}  // namespace lib