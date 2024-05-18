#pragma once
#include <module_core/NoCopy.hpp>

namespace lib
{
//! Classes that inherit singleton can only be accessed using the singleton \c get function.
template <class T>
class Singleton : public NoCopy {
public:
    static T& get() {
        // instance will be created on first use and destroyed when program is killed.
        static T singleton_instance = {};
        return singleton_instance;
    }

private:
    // Ensure constructor and destructor is only callable from the singleton.
    Singleton() = default;
    ~Singleton() = default;

};
}  // namespace lib