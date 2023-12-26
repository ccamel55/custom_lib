#include <lib_gui/lib_gui.hpp>

#include <cassert>

#include <lib_gui/resources/fonts/tahoma.hpp>
#include <lib_gui/resources/fonts/tahomabd.hpp>

using namespace lib::gui;

void gui::init(
	std::weak_ptr<lib::input::input_handler>&& input_handler,
	std::weak_ptr<lib::json_config::config_manager> json_config,
	std::weak_ptr<lib::rendering::renderer> renderer)
{
	_input_handler = std::move(input_handler);
	_json_config = std::move(json_config);
	_renderer = std::move(renderer);

	// register input callbacks
	const auto& input_handler_ptr = _input_handler.lock();
	assert(input_handler_ptr != nullptr);

	input_handler_ptr->register_callback(
		lib::input::mouse | lib::input::keyboard,
		[&](const lib::input::input_handler& processor)
	{

	});

	// register json config callbacks
	const auto& json_config_ptr = _json_config.lock();
	assert(json_config_ptr != nullptr);

	// register fonts and draw callbacks
	const auto& renderer_ptr = _renderer.lock();
	assert(renderer_ptr != nullptr);

	lib::gui::font::title = renderer_ptr->add_font(tahoma_bold_font.data(), 16);
	lib::gui::font::heading = renderer_ptr->add_font(tahoma_bold_font.data(), 14);
	lib::gui::font::text = renderer_ptr->add_font(tahoma_font.data(), 14);

	renderer_ptr->register_callback([&](const lib::rendering::renderer& render)
	{

	});
}

void gui::destroy()
{
	_input_handler = {};
	_json_config = {};
	_renderer = {};
}