#pragma once

#include <array>
#include <functional>

namespace lib::render {

template <
    typename HType,
    typename IType,
    typename Enum,
    size_t Size,
    typename... Args
> requires
    std::is_scoped_enum_v<Enum>
class Creatable {

    // lambda type
    using Fn = std::function<HType(Args...)>;

public:
    //! Get interface type
    //! \param id enum id
    //! \return interface instance ptr for specific enum id
    [[nodiscard]] IType* at(Enum id) const {
        return m_cache[static_cast<size_t>(id)];
    }

    //! Set specific create function
    //! \param id enum id
    //! \param fn create function
    void set(Enum id, Fn&& fn) {
        m_cache[static_cast<size_t>(id)]        = nullptr;
        m_create_fn[static_cast<size_t>(id)]    = std::move(fn);
    }

    //! Call `create_fn` function with latest parameters
    void recreate(Args... args) {
        // Invoke all `create` functions....
        for (size_t i = 0; i < m_cache.size(); i++) {
            m_cache[i] = m_create_fn[i](args...);
        }
    }

protected:
    std::array<HType, Size> m_cache     = {};
    std::array<Fn, Size> m_create_fn    = {};

};

}