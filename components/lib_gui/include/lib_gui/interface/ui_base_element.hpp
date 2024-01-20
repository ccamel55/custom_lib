#pragma once

#include <lib_gui/defines/global_defines.hpp>

namespace lib::gui
{
//! Base class for all drawable UI elements.
class ui_base_element
{
public:
    ui_base_element(const lib::point2Di& pos, const lib::point2Di& size);
    virtual ~ui_base_element() = default;

    //! Draw items in the element.
    virtual void draw(lib::rendering::render_callback_handler& render) = 0;

    //! Handle input for the element, will also refresh if needed.
    virtual void input(const lib::input::input_callback_handler& input) = 0;

    //! Force refresh of the element. Data can be used to pass information to the element.
    virtual void refresh(bitflag update, const void* data) = 0;

    [[nodiscard]] bitflag& flags();

    [[nodiscard]] const lib::point2Di& get_size() const;
    void set_size(const lib::point2Di& size);

    [[nodiscard]] const lib::point2Di& get_position() const;
    void set_position(const lib::point2Di& position);

protected:
    bitflag _flags;

    lib::point2Di _size;
    lib::point2Di _position;

};
}