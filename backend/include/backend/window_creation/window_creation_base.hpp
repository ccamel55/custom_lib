#pragma once

#include <backend/render/renderer_base.hpp>

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
		window_flag_opengl3 = 1 << 1,
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
			}
		}

		//! register a callback function \param render_callback that is called every render frame
		void register_render_callback(std::function<void()> render_callback)
		{
			_render_callback = std::move(render_callback);
		}

		//! register the input renderer used
		void register_renderer(renderer_base* renderer)
		{
			_renderer = renderer;
			_renderer->init_instance();
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
		renderer_base* _renderer = nullptr;

		std::string _window_name = {};

		common::point2Di _window_position = { 100, 100 };
		common::point2Di _window_size = { 720, 480 };

		common::bitflag _flags = {};
	};
}