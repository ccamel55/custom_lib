#pragma once

#include <functional>
#include <string>

#include <lib_rendering/lib_rendering.hpp>

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

class window_creation_base
{
public:
	explicit window_creation_base(const window_parameters_t& window_parameters) :
		_window_parameters(window_parameters), _render_callback(nullptr)
	{
	}

	virtual ~window_creation_base() = default;

	//! register a callback function \param render_callback that is called every render frame
	virtual void register_render_callback(std::function<void()> render_callback)
	{
		_render_callback = std::move(render_callback);
	}

	//! Transfer ownership of the renderer to the window
	//! return a reference to the renderer, because this window owns the renderer, there should never be a case
	//! where our window is destroyed and our renderer still exists
	virtual std::unique_ptr<rendering::renderer>& register_renderer(std::unique_ptr<rendering::renderer> renderer)
	{
		lib_log_d("window_creation: registered renderer");
		_renderer = std::move(renderer);

		return _renderer;
	}

	//! run the loop that handles inputs from the window, this function does not return until the window is closed
	virtual void window_loop() = 0;

	//! close the os window
	virtual void close_window() = 0;

	//! bring the os window to focus
	virtual void focus_window() = 0;

protected:
	window_parameters_t _window_parameters;

	std::function<void()> _render_callback;
	std::unique_ptr<rendering::renderer> _renderer;
};
}  // namespace lib::window_creation