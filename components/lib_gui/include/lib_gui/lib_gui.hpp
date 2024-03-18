#pragma once

#include <memory>
#include <vector>
#include <cassert>

#include <core_sdk/logger/logger.hpp>

#include <lib_gui/defines/global_defines.hpp>
#include <lib_gui/interface/ui_base_window.hpp>

#include <lib_input/lib_input.hpp>
#include <lib_rendering/lib_rendering.hpp>

namespace lib::gui
{
class gui
{
public:
    gui(std::weak_ptr<lib::input::input_handler>&& input_handler, std::weak_ptr<lib::rendering::renderer>&& renderer);

    //! toggle visibility
    void toggle_visibility();

    //! show all windows
    void show_windows();

    //! hide all windows
    void hide_windows();

    [[nodiscard]] bool is_visible() const;

    //! move ownership of a window to the GUI, after calling this, we should only access the window using the weakptr
    //! as the shared pointer used to create the window is not longer guarneteed to be valid
    template<typename t>
    std::weak_ptr<t> register_window(std::shared_ptr<t> window)
    {
        assert(window != nullptr);

        // transfer ownership to the GUI
        const auto ret_val = std::weak_ptr<t>(window);
        const auto window_ptr = _windows.emplace_back(std::move(window));

        lib_log_d("gui: registered a new window, total windows {}", _windows.size());

        // make sure at least one window is focused at all times
        if (_windows.size() == 1)
        {
            set_active(window_ptr);
        }

        // update visibility to match state of UI
        window_ptr->refresh(ui_update::update_visibility, &_show_windows);

		return ret_val;
    }

private:
    void set_active(const std::shared_ptr<ui_base_window>& active_window);

private:
    bool _show_windows = false;

    std::weak_ptr<lib::input::input_handler> _input_handler = {};
    std::weak_ptr<lib::rendering::renderer> _renderer = {};

    //! windows registered to the gui
    std::shared_ptr<ui_base_window> _active_window = nullptr;
    std::vector<std::shared_ptr<ui_base_window>> _windows = {};
};
}