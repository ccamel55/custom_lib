#pragma once

#include <memory>

#include <core_sdk/singleton.hpp>
#include <lib_gui/defines/global_defines.hpp>

#include <lib_input/lib_input.hpp>
#include <lib_json_config/json_config.hpp>
#include <lib_rendering/lib_rendering.hpp>

namespace lib::gui
{
class gui: public singleton<gui>
{
public:
    //! init gui
    void init(
        std::weak_ptr<lib::input::input_handler>&& input_handler,
        std::weak_ptr<lib::json_config::config_manager> json_config,
        std::weak_ptr<lib::rendering::renderer> renderer);

    //! destroy gui/delete from memory and unbind callbacks
    void destroy();

    //! called in render loop, where ever the renderer accepts draw functions
    void on_render();

private:
    std::weak_ptr<lib::input::input_handler> _input_handler = {};
    std::weak_ptr<lib::json_config::config_manager> _json_config = {};
    std::weak_ptr<lib::rendering::renderer> _renderer = {};
};
}