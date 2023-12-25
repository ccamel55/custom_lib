#pragma once

#include <thread>

#include <core_sdk/singleton.hpp>
#include <lib_window_creation/window_api_base.hpp>

namespace lib::window_creation
{
class window_creation: public singleton<window_creation>
{
public:
	//! initalize window creation, treat this as a constructor
	void init(const window_parameters_t& window_parameters);

	//! destroy window creation, treat this as a destructor
	void destroy();

	//! register a callback function \param render_callback that is called every render frame
	void register_render_callback(std::function<void()> render_callback);

#ifndef DEF_LIB_RENDERING_off
	//! Transfer ownership of the renderer to the window
	//! return a reference to the renderer, because this window owns the renderer, there should never be a case
	//! where our window is destroyed and our renderer still exists
	std::unique_ptr<rendering::renderer>& register_renderer(std::unique_ptr<rendering::renderer> renderer);
#endif

#ifndef DEF_LIB_INPUT_off
	//! Transfer ownership of the input handler to the window
	//! return a reference to the input handler, because this window owns the input handler, there should never be a
	//! case where our window is destroyed and our input handler still exists
	std::unique_ptr<input::input_handler>& register_input_handler(std::unique_ptr<input::input_handler> input_handler);
#endif

	//! run the loop that handles inputs from the window, this function does not return until the window is closed
	void run_window_loop();

	//! close the os window
	void close_window() const;

	//! bring the os window to focus
	void focus_window() const;

private:
	bool _init = false;
	bool _window_running = false;

	std::function<void()> _render_callback = nullptr;
	std::unique_ptr<window_api_base> _window_creation_api = nullptr;

#ifndef DEF_LIB_RENDERING_off
	std::unique_ptr<rendering::renderer> _renderer = nullptr;
#endif

#ifndef DEF_LIB_INPUT_off
	std::unique_ptr<input::input_handler> _input_handler = nullptr;
#endif
};
}