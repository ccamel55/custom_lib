#pragma once

#include <module_core/type/point/point4D.hpp>
#include <module_gui/node/UiNode.hpp>

namespace lib::gui {

enum WindowState: bitflag_t {
    WindowState_None        = 0,
    WindowState_Focused     = 1 << 0,
    WindowState_Pinned      = 1 << 1
};

//! Base window node
class WindowNode: public UiNode {

    friend class Gui;

protected:
    //! Set window position safely
    //! \note does check to ensure that the window stays screen bounds
    //! \param pos new position of window
    //! \param bounds bounds window must adhere to
    virtual void SetWindowPosChecked(const point2Di& pos, const point4Di& bounds);

    //! Set window size safely
    //! \note does check to ensure that window stays in
    //! \param size new size of window
    //! \param bounds bounds window must adhere to
    virtual void SetWindowSizeChecked(const point2Di& size, const point4Di& bounds);

    //! Get the dragging area of a window
    //! \note must be inside the window rect
    //! \returns std::nullopt of moving is not supported
    [[nodiscard]] virtual std::optional<point4Di> GetDragArea() const;

    //! Get the resizing area of a window
    //! \note must be inside the window rect
    //! \returns std::nullopt of moving is not supported
    [[nodiscard]] virtual std::optional<point4Di> GetResizeArea() const;

    //! Get current window state
    [[nodiscard]] bitflag& WindowState();

private:
    bitflag m_windowState = WindowState_None;

};

}