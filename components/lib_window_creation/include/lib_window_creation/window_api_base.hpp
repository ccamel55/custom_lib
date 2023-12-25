#pragma once

#include <string>
#include <functional>

#include <core_sdk/logger.hpp>

#ifndef DEF_LIB_RENDERING_off
#include <lib_rendering/lib_rendering.hpp>
#endif

#ifndef DEF_LIB_INPUT_off
#include <lib_input/lib_input.hpp>
#endif

namespace lib::window_creation
{
enum window_flags : uint32_t
{
    window_flag_none = 0 << 0,
    window_flag_no_border = 1 << 0,
    window_flag_resizeable = 1 << 1,
    window_flag_opengl3 = 1 << 2,
};

struct window_parameters_t
{
    std::string window_name;
    int x_position;
    int y_position;
    int width;
    int height;
    uint32_t flags;
};

class window_api_base
{
public:
	explicit window_api_base(std::function<void()> window_loop_callback)
	{
		_window_loop_callback = std::move(window_loop_callback);
	}

	virtual ~window_api_base() = default;

	//! register a callback function \param window_loop_callback that is called every render frame
	virtual void register_render_callback(std::function<void()> window_loop_callback)
	{
		_window_loop_callback = std::move(window_loop_callback);
	}

#ifndef DEF_LIB_RENDERING_off
	//! takes an renderer instance and calls appropriate functions to register it to the window
	virtual bool register_renderer(std::unique_ptr<rendering::renderer>& renderer) = 0;
#endif

#ifndef DEF_LIB_INPUT_off
	//! takes an input handler instance and calls appropriate functions to register it to the window
	virtual bool register_input_handler(std::unique_ptr<input::input_handler>& input_handler) = 0;
#endif

	//! run the loop that handles inputs from the window, this function does not return until the window is closed
	virtual void window_loop() const = 0;

	//! close the os window
	virtual void close_window() const = 0;

	//! bring the os window to focus
	virtual void focus_window() const = 0;

#ifndef DEF_LIB_INPUT_off
	[[nodiscard]] input::input_handler* get_input_handler() const
	{
		return _input_handler_ptr;
	}
#endif

protected:
	std::function<void()> _window_loop_callback = nullptr;

#ifndef DEF_LIB_INPUT_off
	// this should really be a shared/weakptr pair but it should never cause issues so imma leave it as a raw ptr
	input::input_handler* _input_handler_ptr = nullptr;
#endif

};
}