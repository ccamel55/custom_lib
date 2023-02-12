#pragma once

#include <memory>

namespace lib {

    //! singleton used to create one and only one instance of an arbitrary class
    //! \param T class that wants to implement the singleton
    template <class T>
    class singleton {
    public:
        static T& get() {
            // make one, and one instance only
            static T instance{};

            return instance;
        }

        // delete copy methods, prevents creation of another singleton
        singleton(const singleton& s) = delete;
        void operator= (const singleton&) = delete;

    protected:
        // constructor should only be accessible from the singleton class
        singleton() = default;
        ~singleton() = default;
    };

    //! singleton used to create one and only one smart pointer instance of an arbitrary class
    //! \param T class that wants to implement the singleton
    //! \note shared_singleton classes must be instantiated at least once by specifying \param impl
    template <class T>
    class shared_singleton {
    public:
        static std::shared_ptr<T> get(std::shared_ptr<T> impl = nullptr) {

            static std::shared_ptr<T> _impl = nullptr;

            // set implementation
            if (impl != nullptr) {
                _impl = std::move(impl);
            }

            return _impl;
        }
    };
}