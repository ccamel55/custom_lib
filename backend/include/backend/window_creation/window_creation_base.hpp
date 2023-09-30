#pragma once

#include <backend/input_handler/input_handler_base.hpp>
#include <backend/render/renderer_base.hpp>

#include <common/logger.hpp>
#include <common/types/bitflag.hpp>
#include <common/types/point/point2D.hpp>

#include <functional>
#include <string>
#include <utility>

namespace lib::backend
{
enum window_flags : common::bitflag_t
{
	window_flag_none = 0 << 0,
	window_flag_no_border = 1 << 0,
	window_flag_resizeable = 1 << 1,
	window_flag_opengl3 = 1 << 2,
};

//! base class for os window creation
class window_creation_base
{
public:
	window_creation_base(std::string window_name, int pos_x, int pos_y, int width, int height, window_flags flags) :
		_window_name(std::move(window_name)), _window_position(pos_x, pos_y), _window_size(width, height), _flags(flags)
	{
	}

	virtual ~window_creation_base()
	{
		if (_renderer)
		{
			_renderer->destroy_instance();
			_renderer.reset();
		}

		if (_input_handler)
		{
			_input_handler.reset();
		}
	}

	//! register a callback function \param render_callback that is called every render frame
	void register_render_callback(std::function<void()> render_callback)
	{
		_render_callback = std::move(render_callback);
	}

	//! Transfer ownership of the renderer to the window
	//! return a reference to the renderer, because this window owns the renderer, there should never be a case
	//! where our window is destroyed and our renderer still exists
	std::unique_ptr<renderer_base>& register_renderer(std::unique_ptr<renderer_base> renderer)
	{
		lib_log_d("window_creation: registered renderer");
		_renderer = std::move(renderer);

		_renderer->init_instance(nullptr);
		_renderer->set_window_size(_window_size);

		return _renderer;
	}

	//! Transfer ownership of the input handler to the window
	//! return a reference to the input handler, because this window owns the input handler, there should never be a
	//! case where our window is destroyed and our input handler still exists
	std::unique_ptr<input_handler_base>& register_input_handler(std::unique_ptr<input_handler_base> input_handler)
	{
		lib_log_d("window_creation: registered input_handler");
		_input_handler = std::move(input_handler);

		return _input_handler;
	}

	//! return the size of the current window
	[[nodiscard]] const common::point2Di& get_size() const
	{
		return _window_size;
	}

	//! close the os window
	virtual void close_window() = 0;

	//! bring the os window to focus
	virtual void focus_window() = 0;

	//! run the loop that handles inputs from the window
	virtual void window_loop() = 0;

protected:
	std::function<void()> _render_callback = nullptr;

	std::unique_ptr<renderer_base> _renderer = nullptr;
	std::unique_ptr<input_handler_base> _input_handler = nullptr;

	std::string _window_name = {};

	common::point2Di _window_position = {100, 100};
	common::point2Di _window_size = {720, 480};

	common::bitflag _flags = {};
};
}  // namespace lib::backend