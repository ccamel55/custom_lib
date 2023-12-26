#include <lib_gui/lib_gui.hpp>

#include <cassert>

using namespace lib::gui;

void gui::init(
	std::weak_ptr<lib::input::input_handler>&& input_handler,
	std::weak_ptr<lib::json_config::config_manager> json_config,
	std::weak_ptr<lib::rendering::renderer> renderer)
{
	_input_handler = std::move(input_handler);
	_json_config = std::move(json_config);
	_renderer = std::move(renderer);

	const auto& input_handler_ptr = _input_handler.lock();
	assert(input_handler_ptr != nullptr);

	const auto& json_config_ptr = _json_config.lock();
	assert(json_config_ptr != nullptr);

	const auto& renderer_ptr = _renderer.lock();
	assert(renderer_ptr != nullptr);
}

void gui::destroy()
{
	_input_handler = {};
	_json_config = {};
	_renderer = {};
}

void gui::on_render()
{

}