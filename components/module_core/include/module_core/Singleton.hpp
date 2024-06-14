#pragma once

#include <module_core/NoCopy.hpp>

namespace lib {
    //! Helper for creating a singleton from a class.
    template<typename T>
    class Singleton : public NoCopy {
    public:
        static T& get() {
            static T instance{ };
            return instance;
        }

    protected:
        Singleton()  = default;
        ~Singleton() = default;

    };
} // namespace lib
