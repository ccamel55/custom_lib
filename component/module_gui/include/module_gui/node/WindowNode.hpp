#pragma once

#include <module_gui/node/UiNode.hpp>

namespace lib::gui {

struct window_properties_t {
    bool exclusiveRender    = true;
    bool exclusiveInput     = true;
    bool allowInput         = true;
    bool pinnable           = true;
};

enum WindowState: bitflag_t {
    WindowState_None        = 0,
    WindowState_Focused     = 1 << 0,
    WindowState_Pinned      = 1 << 1,
    WindowState_BlockInput  = 1 << 2,
};

//! Base window node
class WindowNode: public UiNode {

    friend class Gui;

public:
    explicit WindowNode(const window_properties_t& properties)
        : m_windowProperties(properties) {

    }

protected:
    //! Get static window properties
    [[nodiscard]] const window_properties_t& GetWindowProperties() const {
        return m_windowProperties;
    }

    //! Get current window state
    [[nodiscard]] bitflag& WindowState() {
        return m_windowState;
    }

private:
    window_properties_t m_windowProperties  = {};
    bitflag m_windowState                   = WindowState_None;

};

}