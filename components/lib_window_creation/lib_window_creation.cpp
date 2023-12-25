#include <lib_window_creation/lib_window_creation.hpp>
#include <lib_window_creation/window_api.hpp>

#include <cassert>

using namespace lib::window_creation;

void window_creation::init(const window_parameters_t& window_parameters)
{
	assert(_init == false);

	const auto window_callback = [&]()
	{
		static auto last_frame_time = std::chrono::high_resolution_clock::now();

#ifndef DEF_LIB_RENDERING_off
		if (_renderer)
		{
			if (_render_callback)
			{
				_render_callback();
			}

			_renderer->draw_frame();
			_renderer->set_frame_time(
				static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - last_frame_time).count()));

			last_frame_time = std::chrono::high_resolution_clock::now();
		}
#endif
	};

	_init = true;
	_window_creation_api = std::make_unique<window_api>(window_parameters, window_callback);
}

void window_creation::destroy()
{
	assert(_init == true);

	_init = false;
	_window_running = false;

	_render_callback = nullptr;
	_window_creation_api = nullptr;

#ifndef DEF_LIB_RENDERING_off
	_renderer = nullptr;
#endif

#ifndef DEF_LIB_INPUT_off
	_input_handler = nullptr;
#endif
}

void window_creation::register_render_callback(std::function<void()> render_callback)
{
	assert(_init == true);
	assert(_renderer != nullptr);

	_render_callback = std::move(render_callback);
}

void window_creation::run_window_loop()
{
	assert(_init == true);
	assert(_window_running == false);

	_window_running = true;
	_window_creation_api->window_loop();
}

void window_creation::close_window() const
{
	assert(_init == true);

	_window_creation_api->close_window();
}

void window_creation::focus_window() const
{
	assert(_init == true);

	_window_creation_api->focus_window();
}

#ifndef DEF_LIB_RENDERING_off
std::unique_ptr<lib::rendering::renderer>& window_creation::register_renderer(
	std::unique_ptr<rendering::renderer> renderer)
{
	assert(_init == true);

	lib_log_d("window_creation: registered renderer");

	_renderer = std::move(renderer);
	_window_creation_api->register_renderer(_renderer);

	return _renderer;
}
#endif

#ifndef DEF_LIB_INPUT_off

std::unique_ptr<lib::input::input_handler>& window_creation::register_input_handler(
	std::unique_ptr<input::input_handler> input_handler)
{
	assert(_init == true);

	lib_log_d("window_creation: registered input_handler");

	_input_handler = std::move(input_handler);
	_window_creation_api->register_input_handler(_input_handler);

	return _input_handler;
}
#endif